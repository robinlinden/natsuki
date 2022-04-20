// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_NATSUKI_SUBSCRIBER_H_
#define NATSUKI_BENCH_NATSUKI_SUBSCRIBER_H_

#include "bench/isubscriber.h"

#include "natsuki/natsuki.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

namespace bench {

class NatsukiSubscriber final : public ISubscriber {
public:
    NatsukiSubscriber(std::string address, std::string topic)
            : nats_{std::move(address)}, topic_{std::move(topic)} {}

    ~NatsukiSubscriber() {
        nats_.shutdown();
        thread_.join();
    }

    void subscribe(std::function<void(std::string_view)> callback) override {
        nats_.subscribe(topic_, std::move(callback));
    }

private:
    natsuki::Nats nats_;
    std::thread thread_{&natsuki::Nats::run, &nats_};
    std::string topic_;
};

class NatsukiSubscriberFactory final : public ISubscriberFactory {
public:
    [[nodiscard]] std::unique_ptr<ISubscriber> create(
            std::string address,
            std::string topic) const override {
        return std::make_unique<NatsukiSubscriber>(std::move(address), std::move(topic));
    }
};

} // namespace bench

#endif
