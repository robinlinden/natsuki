// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "bench/arg_parser.h"

#include "natsuki/natsuki.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace std::literals;

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

struct PartialResult {
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    int messages;
};

} // namespace

int main(int argc, char **argv) try {
    auto address = "localhost"s;
    int msgs = 1'000'000;
    int payload_size = 10;
    auto seed = static_cast<unsigned>(std::time(nullptr));
    int publisher_count = 1;
    int subscriber_count = 0;

    bench::ArgParser()
            .argument("--msgs", msgs)
            .argument("--size", payload_size)
            .argument("--seed", seed)
            .argument("--pub", publisher_count)
            .argument("--sub", subscriber_count)
            .positional(address)
            .parse(argc, argv);

    std::cout << "Benchmarking with seed " << seed
            << " and payload size " << payload_size
            << "B against NATS server at " << address << ".\n";

    std::list<std::thread> threads;
    std::list<natsuki::Nats> subscribers;

    if (subscriber_count > 0) {
        std::cout << "Starting " << subscriber_count << " subscribers, expecting "
                << msgs << " messages each.\n";
    }
    for (int i = 0; i < subscriber_count; ++i) {
        auto &nats = subscribers.emplace_back(address);
        threads.emplace_back(&natsuki::Nats::run, &nats);
    }

    for (auto &nats : subscribers) {
        nats.subscribe("bench"sv, [i = 0, msgs, &nats](std::string_view) mutable {
            i += 1;
            if (i == msgs) {
                nats.shutdown();
            }
        });
    }

    // TODO(robinlinden): Waiting for subscription messages to have propagated
    // in this way shouldn't be needed.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto payload = random_payload(payload_size, seed);

    std::list<natsuki::Nats> publishers;
    std::cout << "Starting " << publisher_count << " publishers, publishing "
            << msgs / publisher_count << " messages each.\n";
    for (int i = 0; i < publisher_count; ++i) {
        auto &nats = publishers.emplace_back(address);
        threads.emplace_back(&natsuki::Nats::run, &nats);
    }

    auto const start = std::chrono::high_resolution_clock::now();
    std::vector<std::future<PartialResult>> runners;
    runners.reserve(publisher_count);
    for (auto &nats : publishers) {
        runners.push_back(std::async(std::launch::async, [&] {
            auto const my_start = std::chrono::high_resolution_clock::now();
            auto const my_msgs = msgs /publisher_count;

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

    for (int i = 0; i < publisher_count; ++i) {
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


    if (publisher_count > 1) {
        for (std::size_t i = 0; i < runners.size(); ++i) {
            auto const res = runners[i].get();
            auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(res.end_time - res.start_time);
            auto const msgs_per_second = static_cast<float>(res.messages) / duration.count() * 1000; // msgs/ms -> msgs/s
            std::cout << std::fixed << "Publisher " << i << " published " << res.messages << " messages in "
                    << duration.count() << "ms. (" << std::setprecision(0) << msgs_per_second << " msgs/s, "
                    << std::setprecision(1) << payload_size * msgs_per_second / 1024.f / 1024.f << "MB/s)\n";
        }
        std::cout << '\n';
    }

    auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto const msgs_per_second = static_cast<float>(msgs) / duration.count() * 1000; // msgs/ms -> msgs/s
    // We publish each message once, and every subscriber reads every message from the server.
    auto const processed = msgs_per_second * (1 + subscriber_count);
    // TODO(robinlinden): Improve message, add per-subscriber/publisher stats.
    std::cout << std::fixed << "Processed " << msgs << " messages in "
            << duration.count() << "ms. (" << std::setprecision(0) << msgs_per_second << " msgs/s, "
            << std::setprecision(1) << payload_size * processed / 1024.f / 1024.f << "MB/s)\n";
} catch (std::exception const &e) {
    std::cerr << e.what();
    throw;
}
