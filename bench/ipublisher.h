// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_IPUBLISHER_H_
#define NATSUKI_BENCH_IPUBLISHER_H_

#include <memory>
#include <string>
#include <string_view>

namespace bench {

class IPublisher {
public:
    virtual ~IPublisher() = default;
    virtual void publish(std::string_view data) = 0;
};

class IPublisherFactory {
public:
    virtual ~IPublisherFactory() = default;
    [[nodiscard]] virtual std::unique_ptr<IPublisher> create(
            std::string address,
            std::string topic) const = 0;
};

} // namespace bench

#endif
