// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_JSON_REPORTER_H_
#define NATSUKI_BENCH_JSON_REPORTER_H_

#include "bench/ibenchmark_listener.h"

#include <cstddef>
#include <iostream>
#include <sstream>

namespace bench {

class JsonReporter final : public IBenchmarkListener {
public:
    void on_benchmark_start(Options const &opts) override {
        auto into_json = [](std::ostream &os, Options const &opts) {
            os << "{\"address\":\"" << opts.address << "\","
                    << "\"messages\":" << opts.messages << ','
                    << "\"payload_size\":" << opts.payload_size << ','
                    << "\"seed\":" << opts.seed << ','
                    << "\"publisher_count\":" << opts.publisher_count << ','
                    << "\"subscriber_count\":" << opts.subscriber_count << '}';
        };

        ss_ << "{\"options\":";
        into_json(ss_, opts);
        ss_ << ',';
    }

    void before_subscriber_start() override {}
    void before_publisher_start() override {}

    void on_publish_done(std::vector<PartialResult> const &results) override {
        ss_ << "\"publish_results\":" << '[';
        for (std::size_t i = 0; i < results.size(); ++i) {
            if (i > 0) {
                ss_ << ',';
            }

            to_json(ss_, results[i]);
        }
        ss_ << "],";
    }

    void on_subscribe_done(std::vector<PartialResult> const &results) override {
        ss_ << "\"subscribe_results\":" << '[';
        for (std::size_t i = 0; i < results.size(); ++i) {
            if (i > 0) {
                ss_ << ',';
            }

            to_json(ss_, results[i]);
        }
        ss_ << "],";
    }

    void on_benchmark_done(std::chrono::milliseconds duration) override {
        ss_ << "\"duration\":" << duration.count() << "}";
        std::cout << ss_.str();
    }

private:
    void to_json(std::ostream &os, PartialResult const &res) {
        os << "{\"start_time\":" << res.start_time.time_since_epoch().count() << ','
                << "\"end_time\":" << res.end_time.time_since_epoch().count() << ','
                << "\"messages\":" << res.messages << '}';
    }

    std::stringstream ss_;
};

} // namespace bench

#endif
