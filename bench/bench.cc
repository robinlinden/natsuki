// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "bench/arg_parser.h"
#include "bench/ibenchmark_listener.h"
#include "bench/options.h"
#include "bench/partial_result.h"
#include "bench/stdout_listener.h"

#include "natsuki/natsuki.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace std::literals;

namespace bench {
namespace {

std::string random_payload(int length, unsigned seed) {
    static constexpr auto chars =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "!#$%&'()*+,-./:;<=>?@[]^_`{|}~"
            "\"\\";
    auto gen = std::mt19937{};
    gen.seed(seed);
    auto dist = std::uniform_int_distribution{std::size_t{0}, std::strlen(chars) - 1};
    auto result = std::string(length, '\0');
    std::generate_n(begin(result), length, [&] { return chars[dist(gen)]; });
    return result;
}

void run_bench(IBenchmarkListener &listener, Options const opts) {
    listener.on_benchmark_start(opts);

    std::list<std::thread> threads;
    std::list<natsuki::Nats> subscribers;
    std::vector<PartialResult> subscriber_results(opts.subscriber_count);

    listener.before_subscriber_start();
    for (int i = 0; i < opts.subscriber_count; ++i) {
        auto &nats = subscribers.emplace_back(opts.address);
        threads.emplace_back(&natsuki::Nats::run, &nats);
    }

    for (int i = 0; i < opts.subscriber_count; ++i) {
        auto &nats = *std::next(begin(subscribers), i);
        nats.subscribe(
                "bench"sv,
                [msg = 0, msgs = opts.messages, &nats , idx = i, &subscriber_results](std::string_view) mutable {
            if (msg == 0) {
                subscriber_results[idx].start_time = std::chrono::high_resolution_clock::now();
            }

            msg += 1;

            if (msg == msgs) {
                subscriber_results[idx].end_time = std::chrono::high_resolution_clock::now();
                subscriber_results[idx].messages = msgs;
                nats.shutdown();
            }
        });
    }

    // TODO(robinlinden): Waiting for subscription messages to have propagated
    // in this way shouldn't be needed.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto payload = random_payload(opts.payload_size, opts.seed);

    std::list<natsuki::Nats> publishers;
    listener.before_publisher_start();
    for (int i = 0; i < opts.publisher_count; ++i) {
        auto &nats = publishers.emplace_back(opts.address);
        threads.emplace_back(&natsuki::Nats::run, &nats);
    }

    auto const start = std::chrono::high_resolution_clock::now();
    std::vector<std::future<PartialResult>> runners;
    runners.reserve(opts.publisher_count);
    for (auto &nats : publishers) {
        runners.push_back(std::async(std::launch::async, [&] {
            auto const my_start = std::chrono::high_resolution_clock::now();
            auto const my_msgs = opts.messages / opts.publisher_count;

            for (int j = 0; j < my_msgs; ++j) {
                nats.publish("bench"sv, payload);
            }

            auto const my_end = std::chrono::high_resolution_clock::now();

            return PartialResult{
                .start_time = my_start,
                .end_time = my_end,
                .messages = my_msgs,
            };
        }));
    }

    for (int i = 0; i < opts.publisher_count; ++i) {
        runners[i].wait();
    }

    for (std::size_t i = 0; i < subscribers.size(); ++i) {
        threads.front().join();
        threads.pop_front();
    }

    auto const end = std::chrono::high_resolution_clock::now();

    for (auto &nats : publishers) {
        nats.shutdown();
        threads.front().join();
        threads.pop_front();
    }

    std::vector<PartialResult> publisher_results{};
    publisher_results.reserve(runners.size());
    std::transform(
            runners.begin(), runners.end(), std::back_inserter(publisher_results),
            [](auto &res) { return res.get(); });
    listener.on_publish_done(publisher_results);
    listener.on_subscribe_done(subscriber_results);
    auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    listener.on_benchmark_done(duration);
}

} // namespace
} // namespace bench

int main(int argc, char **argv) try {
    bench::Options opts;

    bench::ArgParser()
            .argument("--msgs", opts.messages)
            .argument("--size", opts.payload_size)
            .argument("--seed", opts.seed)
            .argument("--pub", opts.publisher_count)
            .argument("--sub", opts.subscriber_count)
            .positional(opts.address)
            .parse(argc, argv);

    bench::StdoutListener listener{};
    bench::run_bench(listener, std::move(opts));
} catch (std::exception const &e) {
    std::cerr << e.what();
    throw;
}
