// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_STDOUT_LISTENER_H_
#define NATSUKI_BENCH_STDOUT_LISTENER_H_

#include "bench/ibenchmark_listener.h"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>

namespace bench {

class StdoutListener final : public IBenchmarkListener {
public:
    void on_benchmark_start(Options const &opts) override {
        opts_ = opts;
        std::cout << "Benchmarking with seed " << opts_.seed
                << " and payload size " << opts_.payload_size
                << "B against NATS server at " << opts_.address << ".\n";
    }

    void before_subscriber_start() override {
        if (opts_.subscriber_count == 0) {
            return;
        }

        std::cout << "Starting " << opts_.subscriber_count << " subscribers, expecting "
                << opts_.messages << " messages each.\n";
    }

    void before_publisher_start() override {
        std::cout << "Starting " << opts_.publisher_count << " publishers, publishing "
                << opts_.messages / opts_.publisher_count << " messages each.\n";
    }

    void on_publish_done(std::vector<PartialResult> const &results) override {
        for (std::size_t i = 0; i < results.size(); ++i) {
            auto const res = results[i];
            auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(res.end_time - res.start_time);
            auto const msgs_per_second = static_cast<float>(res.messages) / duration.count() * 1000; // msgs/ms -> msgs/s
            std::cout << std::fixed << "Publisher " << i << " published " << res.messages << " messages in "
                    << duration.count() << "ms. (" << std::setprecision(0) << msgs_per_second << " msgs/s, "
                    << std::setprecision(1) << opts_.payload_size * msgs_per_second / 1024.f / 1024.f << "MB/s)\n";
        }
    }

    void on_subscribe_done(std::vector<PartialResult> const &results) override {
        for (int i = 0; i < opts_.subscriber_count; ++i) {
            auto const res = results[i];
            auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(res.end_time - res.start_time);
            auto const msgs_per_second = static_cast<float>(res.messages) / duration.count() * 1000; // msgs/ms -> msgs/s
            std::cout << std::fixed << "Subscriber " << i << " handled " << res.messages << " messages in "
                    << duration.count() << "ms. (" << std::setprecision(0) << msgs_per_second << " msgs/s, "
                    << std::setprecision(1) << opts_.payload_size * msgs_per_second / 1024.f / 1024.f << "MB/s)\n";
        }
        std::cout << '\n';
    }

    void on_benchmark_done(std::chrono::milliseconds duration) override {
        auto const msgs_per_second = static_cast<float>(opts_.messages) / duration.count() * 1000; // msgs/ms -> msgs/s
        // We publish each message once, and every subscriber reads every message from the server.
        auto const processed = msgs_per_second * (1 + opts_.subscriber_count);
        // TODO(robinlinden): Improve message, add per-subscriber/publisher stats.
        std::cout << std::fixed << "Processed " << opts_.messages << " messages in "
                << duration.count() << "ms. (" << std::setprecision(0) << msgs_per_second << " msgs/s, "
                << std::setprecision(1) << opts_.payload_size * processed / 1024.f / 1024.f << "MB/s)\n";
    }

private:
    Options opts_{};
};

} // namespace bench

#endif
