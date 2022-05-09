// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "bench/arg_parser.h"
#include "bench/bench.h"
#include "bench/json_reporter.h"
#ifdef NATSC_BENCH
#include "bench/natsc_publisher.h"
#include "bench/natsc_subscriber.h"
#else
#include "bench/natsuki_publisher.h"
#include "bench/natsuki_subscriber.h"
#endif
#include "bench/options.h"
#include "bench/plain_text_reporter.h"

#include <exception>
#include <iostream>
#include <memory>
#include <utility>

int main(int argc, char **argv) try {
    bench::Options opts;
    bool want_json = false;

    bench::ArgParser()
            .argument("--msgs", opts.messages)
            .argument("--size", opts.payload_size)
            .argument("--seed", opts.seed)
            .argument("--pub", opts.publisher_count)
            .argument("--sub", opts.subscriber_count)
            .argument("--msgs-per-second", opts.messages_per_second)
            .argument("--json", want_json)
            .positional(opts.address)
            .parse(argc, argv);

    auto listener = [&]() -> std::unique_ptr<bench::IBenchmarkListener> {
        if (want_json) {
            return std::make_unique<bench::JsonReporter>();
        }
        return std::make_unique<bench::PlainTextReporter>();
    }();

#ifdef NATSC_BENCH
    bench::NatscPublisherFactory pub_factory{};
    bench::NatscSubscriberFactory sub_factory{};
#else
    bench::NatsukiPublisherFactory pub_factory{};
    bench::NatsukiSubscriberFactory sub_factory{};
#endif
    bench::run_bench(*listener, std::move(opts), pub_factory, sub_factory);
} catch (std::exception const &e) {
    std::cerr << e.what();
    throw;
}
