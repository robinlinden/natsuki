// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "bench/arg_parser.h"
#include "bench/bench.h"
#include "bench/natsuki_publisher.h"
#include "bench/natsuki_subscriber.h"
#include "bench/options.h"
#include "bench/stdout_listener.h"

#include <exception>
#include <iostream>
#include <utility>

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
    bench::NatsukiPublisherFactory pub_factory{};
    bench::NatsukiSubscriberFactory sub_factory{};
    bench::run_bench(listener, std::move(opts), pub_factory, sub_factory);
} catch (std::exception const &e) {
    std::cerr << e.what();
    throw;
}
