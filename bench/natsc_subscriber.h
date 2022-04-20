// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_NATSC_SUBSCRIBER_H_
#define NATSUKI_BENCH_NATSC_SUBSCRIBER_H_

#include "bench/isubscriber.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

typedef struct __natsConnection natsConnection;
typedef struct __natsSubscription natsSubscription;

namespace bench {

class NatscSubscriber : public ISubscriber {
public:
    NatscSubscriber(std::string address, std::string topic);
    ~NatscSubscriber() override;

    void subscribe(std::function<void(std::string_view)> on_data) override;

    using CallbackT = std::function<void(std::string_view)>;
private:
    std::string topic_{};
    CallbackT on_data_{};
    natsConnection *conn_{nullptr};
    natsSubscription *sub_{nullptr};
};

class NatscSubscriberFactory : public ISubscriberFactory {
public:
    std::unique_ptr<ISubscriber> create(std::string address, std::string topic) const override {
        return std::make_unique<NatscSubscriber>(std::move(address), std::move(topic));
    }
};

} // namespace bench

#endif
