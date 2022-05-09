// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "bench/bench.h"

#include "bench/ibenchmark_listener.h"
#include "bench/ipublisher.h"
#include "bench/isubscriber.h"
#include "bench/options.h"
#include "bench/publish_result.h"
#include "bench/subscribe_result.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <future>
#include <iterator>
#include <list>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace std::literals;

namespace bench {
namespace {

[[nodiscard]] std::string random_payload(int length, unsigned seed) {
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

constexpr auto as_us(auto const duration) {
    return std::chrono::duration_cast<std::chrono::microseconds>(duration);
}

auto time_since_epoch() {
    return std::chrono::high_resolution_clock::now().time_since_epoch();
}

} // namespace

void run_bench(
        IBenchmarkListener &listener,
        Options const opts,
        IPublisherFactory const &publisher_factory,
        ISubscriberFactory const &subscriber_factory) {
    listener.on_benchmark_start(opts);

    std::vector<std::unique_ptr<ISubscriber>> subscribers;
    std::vector<SubscribeResult> subscriber_results(opts.subscriber_count);

    listener.before_subscriber_start();
    for (int i = 0; i < opts.subscriber_count; ++i) {
        subscribers.emplace_back(subscriber_factory.create(opts.address, "bench"s));
    }
    std::condition_variable subscribers_cv{};
    std::mutex subscribers_mtx{};
    std::size_t subscribers_done{0};

    for (int i = 0; i < opts.subscriber_count; ++i) {
        subscribers[i]->subscribe([&, msg = 0, msgs = opts.messages, idx = i](auto) mutable {
            if (msg == 0) {
                subscriber_results[idx].start_time = std::chrono::high_resolution_clock::now();
            }

            msg += 1;

            if (msg == msgs) {
                subscriber_results[idx].end_time = std::chrono::high_resolution_clock::now();
                subscriber_results[idx].messages = msgs;
                {
                    std::scoped_lock<std::mutex> lock{subscribers_mtx};
                    subscribers_done += 1;
                }
                subscribers_cv.notify_all();
            }
        });
    }

    // TODO(robinlinden): Waiting for subscription messages to have propagated
    // in this way shouldn't be needed.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto payload = random_payload(opts.payload_size, opts.seed);

    std::vector<std::unique_ptr<IPublisher>> publishers;
    listener.before_publisher_start();
    for (int i = 0; i < opts.publisher_count; ++i) {
        publishers.emplace_back(publisher_factory.create(opts.address, "bench"s));
    }

    auto const start = std::chrono::high_resolution_clock::now();
    std::vector<std::future<PublishResult>> runners;
    runners.reserve(opts.publisher_count);
    for (auto &publisher : publishers) {
        runners.push_back(std::async(std::launch::async, [&] {
            auto const my_start = std::chrono::high_resolution_clock::now();
            auto const my_msgs = opts.messages / opts.publisher_count;
            auto const time_per_message = opts.messages_per_second > 0
                    ? as_us(std::chrono::seconds(1)) / opts.messages_per_second
                    : std::chrono::seconds(0);

            for (int j = 0; j < my_msgs; ++j) {
                auto sent = as_us(time_since_epoch()).count();
                publisher->publish(payload);

                auto next = sent + time_per_message.count();
                while (as_us(time_since_epoch()).count() < next) {
                    std::this_thread::yield();
                }
            }

            auto const my_end = std::chrono::high_resolution_clock::now();

            return PublishResult{
                .start_time = my_start,
                .end_time = my_end,
                .messages = my_msgs,
            };
        }));
    }

    for (int i = 0; i < opts.publisher_count; ++i) {
        runners[i].wait();
    }

    {
        std::unique_lock<std::mutex> lock{subscribers_mtx};
        subscribers_cv.wait(lock, [&] { return subscribers_done == subscribers.size(); });
    }

    auto const end = std::chrono::high_resolution_clock::now();

    while (!subscribers.empty()) {
        subscribers.pop_back();
    }

    while (!publishers.empty()) {
        publishers.pop_back();
    }

    std::vector<PublishResult> publisher_results{};
    publisher_results.reserve(runners.size());
    std::transform(
            runners.begin(), runners.end(), std::back_inserter(publisher_results),
            [](auto &res) { return res.get(); });
    listener.on_publish_done(publisher_results);
    listener.on_subscribe_done(subscriber_results);
    auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    listener.on_benchmark_done(duration);
}

} // namespace bench
