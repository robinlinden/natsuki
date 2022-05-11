// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_SUBSCRIBE_RESULT_H_
#define NATSUKI_BENCH_SUBSCRIBE_RESULT_H_

#include <chrono>
#include <map>

namespace bench {

struct SubscribeResult {
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    int messages;
    std::map<unsigned, unsigned> latencies; // latency -> count
};

} // namespace bench

#endif
