// SPDX-FileCopyrightText: 2021-2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

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

    nats.subscribe("natsuki.qgroup", [](auto data) {
        std::cout << "qgroup listener 1: " << data << '\n';
    }, { .queue_group = "G1", });
    nats.subscribe("natsuki.qgroup", [](auto data) {
        std::cout << "qgroup listener 2: " << data << '\n';
    }, { .queue_group = "G1", });

    nats.subscribe("natsuki.unsubscribed_after", [](auto data) {
        std::cout << "this unsubscribes after receiving 1 message: " << data << '\n';
    }, { .unsubscribe_after = 1, });

    nats.publish("natsuki.running", "true");
    nats.publish("natsuki.ping", "1", "natsuki.pong");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    nats.unsubscribe(std::move(ping_sub)).wait();
    nats.publish("natsuki.ping", "2");
    nats.publish("natsuki.unsubscribed_after", "first!");
    nats.publish("natsuki.unsubscribed_after", "this shouldn't be received");
    nats.publish("natsuki.unsubscribed_after", "nor should this be");
    nats.publish("natsuki.qgroup", "hello?");
    nats.publish("natsuki.qgroup", "hello!");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    nats.shutdown();
    nats_thread.join();
} catch (std::runtime_error const &e) {
    std::cerr << e.what();
    throw;
}
