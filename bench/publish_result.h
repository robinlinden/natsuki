// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_PUBLISH_RESULT_H_
#define NATSUKI_BENCH_PUBLISH_RESULT_H_

#include <chrono>

namespace bench {

struct PublishResult {
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    int messages;
};

} // namespace bench

#endif
