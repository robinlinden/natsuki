#include "natsuki/natsuki.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <utility>

int main(int argc, char **argv) try {
    natsuki::Nats nats{argc >= 2 ? argv[1] : "localhost"};
    std::thread nats_thread{&natsuki::Nats::run, &nats};

    nats.subscribe("natsuki.running", [](std::string_view data) {
        std::cout << "running: " << data << "\n";
    });

    nats.subscribe("natsuki.not_running", [](std::string_view data) {
        std::cout << "not_running: " << data << "\n";
    });

    auto ping_sub = nats.subscribe("natsuki.ping", [](std::string_view data) {
        std::cout << "ping 1: " << data << "\n";
    });

    nats.subscribe("natsuki.ping", [](std::string_view data) {
        std::cout << "ping 2: " << data << "\n";
    });

    nats.publish("natsuki.running", "true");
    nats.publish("natsuki.ping", "1", "natsuki.pong");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    nats.unsubscribe(std::move(ping_sub)).wait();
    nats.publish("natsuki.ping", "2");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    nats.shutdown();
    nats_thread.join();
} catch (std::runtime_error const &e) {
    std::cerr << e.what();
    throw;
}
