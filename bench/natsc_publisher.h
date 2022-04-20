// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_NATSC_PUBLISHER_H_
#define NATSUKI_BENCH_NATSC_PUBLISHER_H_

#include "bench/ipublisher.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

typedef struct __natsConnection natsConnection;

namespace bench {

class NatscPublisher : public IPublisher {
public:
    NatscPublisher(std::string address, std::string topic);
    ~NatscPublisher() override;

    void publish(std::string_view data) override;

private:
    std::string topic_{};
    natsConnection *conn_{nullptr};
};

class NatscPublisherFactory : public IPublisherFactory {
public:
    std::unique_ptr<IPublisher> create(std::string address, std::string topic) const override {
        return std::make_unique<NatscPublisher>(std::move(address), std::move(topic));
    }
};

} // namespace bench

#endif
