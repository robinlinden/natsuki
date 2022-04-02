// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "natsuki/natsuki.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

using namespace std::literals;

int main(int argc, char **argv) try {
    auto address = "localhost"s;
    int msgs = 1'000'000;

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == "--msgs"sv) {
            if (i + 1 == argc) {
                std::cout << "Missing count after --msgs\n";
                return 1;
            }

            msgs = std::stoi(argv[i + 1]);
            ++i;
            continue;
        }

        // Positional.
        if (i == argc - 1) {
            address = argv[i];
        }
    }

    std::cout << "Benchmarking against NATS server at " << address << ".\n";
    natsuki::Nats nats{std::move(address)};
    std::thread nats_thread{&natsuki::Nats::run, &nats};

    auto const start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < msgs; ++i) {
        nats.publish("bench"sv, "data"sv);
    }
    auto const end = std::chrono::high_resolution_clock::now();
    std::cout << "Published " << msgs << " messages in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms.\n";

    nats.shutdown();
    nats_thread.join();
} catch (std::exception const &e) {
    std::cerr << e.what();
    throw;
}
