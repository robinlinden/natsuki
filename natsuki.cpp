#include "natsuki.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

int main(int argc, char **argv) try {
    natsuki::Nats nats{argc >= 2 ? argv[1] : "localhost"};
    std::thread nats_thread{&natsuki::Nats::run, &nats};
    std::this_thread::sleep_for(std::chrono::seconds(5));
    nats.shutdown();
    nats_thread.join();
} catch (std::runtime_error const &e) {
    std::cerr << e.what();
    throw;
}
