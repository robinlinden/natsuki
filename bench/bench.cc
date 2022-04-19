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
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
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

class IPublisher {
public:
    virtual ~IPublisher() = default;
    virtual void publish(std::string_view data) = 0;
};

class ISubscriber {
public:
    virtual ~ISubscriber() = default;
    virtual void subscribe(std::function<void(std::string_view)> on_data) = 0;
};

class Publisher final : public IPublisher {
public:
    Publisher(std::string address, std::string topic)
            : nats_{std::move(address)}, topic_{std::move(topic)} {}

    ~Publisher() {
        nats_.shutdown();
        thread_.join();
    }

    void publish(std::string_view data) override {
        nats_.publish(topic_, data);
    }

private:
    natsuki::Nats nats_;
    std::thread thread_{&natsuki::Nats::run, &nats_};
    std::string topic_;
};

class Subscriber final : public ISubscriber {
public:
    Subscriber(std::string address, std::string topic)
            : nats_{std::move(address)}, topic_{std::move(topic)} {}

    ~Subscriber() {
        nats_.shutdown();
        thread_.join();
    }

    void subscribe(std::function<void(std::string_view)> callback) override {
        nats_.subscribe(topic_, std::move(callback));
    }

private:
    natsuki::Nats nats_;
    std::thread thread_{&natsuki::Nats::run, &nats_};
    std::string topic_;
};

void run_bench(IBenchmarkListener &listener, Options const opts) {
    listener.on_benchmark_start(opts);

    std::vector<std::unique_ptr<ISubscriber>> subscribers;
    std::vector<PartialResult> subscriber_results(opts.subscriber_count);

    listener.before_subscriber_start();
    for (int i = 0; i < opts.subscriber_count; ++i) {
        subscribers.emplace_back(std::make_unique<Subscriber>(opts.address, "bench"s));
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
        publishers.emplace_back(std::make_unique<Publisher>(opts.address, "bench"s));
    }

    auto const start = std::chrono::high_resolution_clock::now();
    std::vector<std::future<PartialResult>> runners;
    runners.reserve(opts.publisher_count);
    for (auto &publisher : publishers) {
        runners.push_back(std::async(std::launch::async, [&] {
            auto const my_start = std::chrono::high_resolution_clock::now();
            auto const my_msgs = opts.messages / opts.publisher_count;

            for (int j = 0; j < my_msgs; ++j) {
                publisher->publish(payload);
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
