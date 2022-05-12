// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_OPTIONS_H_
#define NATSUKI_BENCH_OPTIONS_H_

#include <ctime>
#include <string>

namespace bench {

struct Options {
    std::string address{"localhost"};
    int messages{250'000};
    int payload_size{1024};
    unsigned seed{static_cast<unsigned>(std::time(nullptr))};
    int publisher_count{1};
    int subscriber_count{1};
    unsigned messages_per_second{100'000};
};

} // namespace bench

#endif
