// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_BENCHMARK_LISTENER_H_
#define NATSUKI_BENCH_BENCHMARK_LISTENER_H_

#include "bench/options.h"
#include "bench/publish_result.h"
#include "bench/subscribe_result.h"

#include <chrono>
#include <vector>

namespace bench {

class IBenchmarkListener {
public:
    virtual ~IBenchmarkListener() = default;
    virtual void on_benchmark_start(Options const &) = 0;
    virtual void before_subscriber_start() = 0;
    virtual void before_publisher_start() = 0;
    virtual void on_publish_done(std::vector<PublishResult> const &) = 0;
    virtual void on_subscribe_done(std::vector<SubscribeResult> const &) = 0;
    virtual void on_benchmark_done(std::chrono::milliseconds duration) = 0;
};

} // namespace bench

#endif
