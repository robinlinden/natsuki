#include "natsuki/natsuki.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

using namespace std::literals;

int main(int argc, char **argv) try {
    natsuki::Nats nats{argc >= 2 ? argv[1] : "localhost"};
    std::thread nats_thread{&natsuki::Nats::run, &nats};

    auto const start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1'000'000; ++i) {
        nats.publish("bench"sv, "data"sv);
    }
    auto const end = std::chrono::high_resolution_clock::now();
    std::cout << "Published 1'000'000 messages in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "ms.\n";

    nats.shutdown();
    nats_thread.join();
} catch (std::runtime_error const &e) {
    std::cerr << e.what();
    throw;
}
