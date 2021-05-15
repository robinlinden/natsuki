#include "natsuki.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

int main(int argc, char **argv) try {
    natsuki::Nats nats{argc >= 2 ? argv[1] : "localhost"};
    std::thread nats_thread{&natsuki::Nats::run, &nats};

    nats.subscribe("natsuki.running", [](std::string_view data) {
        std::cout << "running: " << data << "\n";
    });

    nats.subscribe("natsuki.not_running", [](std::string_view data) {
        std::cout << "not_running: " << data << "\n";
    });

    nats.subscribe("natsuki.ping", [](std::string_view data) {
        std::cout << "ping 1: " << data << "\n";
    });

    nats.subscribe("natsuki.ping", [](std::string_view data) {
        std::cout << "ping 2: " << data << "\n";
    });

    nats.publish("natsuki.running", "true");
    nats.publish("natsuki.ping", "", "natsuki.pong");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    nats.shutdown();
    nats_thread.join();
} catch (std::runtime_error const &e) {
    std::cerr << e.what();
    throw;
}
