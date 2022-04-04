// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "natsuki/natsuki.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <list>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

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
    int publisher_count = 1;

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

        if (argv[i] == "--pub"sv) {
            if (i + 1 == argc) {
                std::cout << "Missing count after --pub\n";
                return 1;
            }

            publisher_count = std::stoi(argv[i + 1]);
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

    std::list<natsuki::Nats> publishers;
    std::list<std::thread> threads;
    for (int i = 0; i < publisher_count; ++i) {
        auto &nats = publishers.emplace_back(address);
        threads.emplace_back(&natsuki::Nats::run, &nats);
    }

    auto const start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> runners;
    runners.reserve(publisher_count);
    for (auto &nats : publishers) {
        runners.emplace_back([&] {
            for (int j = 0; j < msgs / publisher_count; ++j) {
                nats.publish("bench"sv, payload);
            }
        });
    }

    for (int i = 0; i < publisher_count; ++i) {
        runners[i].join();
    }

    auto const end = std::chrono::high_resolution_clock::now();

    for (auto &nats : publishers) {
        nats.shutdown();
        threads.front().join();
        threads.pop_front();
    }

    auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto const msgs_per_second = static_cast<float>(msgs) / duration.count() * 1000; // msgs/ms -> msgs/s
    std::cout << std::fixed << "Published " << msgs << " messages in "
            << duration.count() << "ms. (" << std::setprecision(0) << msgs_per_second << " msgs/s, "
            << std::setprecision(1) << payload_size * msgs_per_second / 1024.f / 1024.f << "MB/s)\n";
} catch (std::exception const &e) {
    std::cerr << e.what();
    throw;
}
