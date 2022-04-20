// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_NATSUKI_PUBLISHER_H_
#define NATSUKI_BENCH_NATSUKI_PUBLISHER_H_

#include "bench/ipublisher.h"

#include "natsuki/natsuki.h"

#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace bench {

class NatsukiPublisher final : public IPublisher {
public:
    NatsukiPublisher(std::string address, std::string topic)
            : nats_{std::move(address)}, topic_{std::move(topic)} {}

    ~NatsukiPublisher() {
        nats_.shutdown();
        thread_.join();
    }

    void publish(std::string_view data) override {
        nats_.publish(topic_, data);
    }

private:
    natsuki::Nats nats_;
    std::thread thread_{&natsuki::Nats::run, &nats_};
    std::string topic_;
};

class NatsukiPublisherFactory final : public IPublisherFactory {
public:
    [[nodiscard]] std::unique_ptr<IPublisher> create(
            std::string address,
            std::string topic) const override {
        return std::make_unique<NatsukiPublisher>(std::move(address), std::move(topic));
    }
};

} // namespace bench

#endif
