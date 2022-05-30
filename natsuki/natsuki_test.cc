// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "natsuki/natsuki.h"

#include "etest/etest.h"

#include <chrono>
#include <thread>

int main() {
    etest::test("destruction without a connection", [] {
        natsuki::Nats nats{"localhost"};
    });

    // We'll share this NATS client for the rest of the tests.
    natsuki::Nats nats{"localhost"};
    std::thread nats_thread{&natsuki::Nats::run, &nats};

    etest::test("crlf in payload", [&] {
        std::atomic_bool got_msg{false};
        nats.subscribe("crlf_in_payload", [&](auto data) {
            etest::expect_eq(data, "\r\n hello!");
            got_msg = true;
        });
        nats.publish("crlf_in_payload", "\r\n hello!");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        etest::expect(got_msg);
    });

    auto ret = etest::run_all_tests();
    nats.shutdown();
    nats_thread.join();
    return ret;
}
