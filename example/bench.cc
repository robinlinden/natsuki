// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "natsuki/natsuki.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

using namespace std::literals;

namespace {

std::string random_payload(int length, unsigned seed) {
    static constexpr auto chars =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "!#$%&'()*+,-./:;<=>?@[]^_`{|}~"
            "\"\\";
    auto gen = std::mt19937{};
    gen.seed(seed);
    auto dist = std::uniform_int_distribution{std::size_t{0}, std::strlen(chars) - 1};
    auto result = std::string(length, '\0');
    std::generate_n(begin(result), length, [&] { return chars[dist(gen)]; });
    return result;
}

} // namespace

int main(int argc, char **argv) try {
    auto address = "localhost"s;
    int msgs = 1'000'000;
    int payload_size = 10;
    auto seed = static_cast<unsigned>(std::time(nullptr));

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

        if (argv[i] == "--size"sv) {
            if (i + 1 == argc) {
                std::cout << "Missing count after --size\n";
                return 1;
            }

            payload_size = std::stoi(argv[i + 1]);
            ++i;
            continue;
        }

        if (argv[i] == "--seed"sv) {
            if (i + 1 == argc) {
                std::cout << "Missing count after --seed\n";
                return 1;
            }

            seed = std::stoi(argv[i + 1]);
            ++i;
            continue;
        }

        // Positional.
        if (i == argc - 1) {
            address = argv[i];
        }
    }

    std::cout << "Benchmarking with seed " << seed
            << " and payload size " << payload_size
            << " against NATS server at " << address << ".\n";

    auto payload = random_payload(payload_size, seed);
    natsuki::Nats nats{std::move(address)};
    std::thread nats_thread{&natsuki::Nats::run, &nats};

    auto const start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < msgs; ++i) {
        nats.publish("bench"sv, payload);
    }
    auto const end = std::chrono::high_resolution_clock::now();
    auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto const msgs_per_second = static_cast<float>(msgs) / duration.count() * 1000; // msgs/ms -> msgs/s
    std::cout << std::fixed << std::setprecision(0)
            << "Published " << msgs << " messages in "
            << duration.count() << "ms. (" << msgs_per_second << " msgs/s)\n";

    nats.shutdown();
    nats_thread.join();
} catch (std::exception const &e) {
    std::cerr << e.what();
    throw;
}
