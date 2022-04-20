// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_ISUBSCRIBER_H_
#define NATSUKI_BENCH_ISUBSCRIBER_H_

#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace bench {

class ISubscriber {
public:
    virtual ~ISubscriber() = default;
    virtual void subscribe(std::function<void(std::string_view)> on_data) = 0;
};

class ISubscriberFactory {
public:
    virtual ~ISubscriberFactory() = default;
    [[nodiscard]] virtual std::unique_ptr<ISubscriber> create(
            std::string address,
            std::string topic) const = 0;
};

} // namespace bench

#endif
