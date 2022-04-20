// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_BENCH_H_
#define NATSUKI_BENCH_BENCH_H_

#include "bench/ibenchmark_listener.h"
#include "bench/ipublisher.h"
#include "bench/isubscriber.h"
#include "bench/options.h"

namespace bench {

void run_bench(
        IBenchmarkListener &,
        Options opts,
        IPublisherFactory const &,
        ISubscriberFactory const &);

} // namespace bench

#endif
