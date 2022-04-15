// SPDX-FileCopyrightText: 2021-2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_NATSUKI_NATSUKI_H_
#define NATSUKI_NATSUKI_NATSUKI_H_

#include <asio.hpp>

#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace natsuki {

class Subscription {
public:
    Subscription(int id) : id_{id} {}
    int id() const { return id_; }

private:
    int id_{};
};

struct SubscriptionOptions {
    static constexpr unsigned kNever = 0;
    unsigned unsubscribe_after{kNever};
    std::string_view queue_group{};
};

class Nats {
public:
    explicit Nats(std::string address, std::uint16_t port = 4222)
            : address_{std::move(address)}, port_{port} {}

    // Run the internal asio io context. Will return only once the io context
    // has been stopped.
    void run();

    // Stop the internal asio context.
    void shutdown();

    void publish(
            std::string_view subject,
            std::string_view payload,
            std::optional<std::string_view> reply_to = std::nullopt);

    Subscription subscribe(
            std::string_view subject,
            std::function<void(std::string_view)> cb,
            SubscriptionOptions = {});
    std::future<void> unsubscribe(Subscription &&);

    Nats(Nats const &) = delete;
    Nats &operator=(Nats const &) = delete;

private:
    void on_read(asio::error_code const &ec, std::size_t bytes_transferred);

    asio::io_context io_context_{};
    asio::ip::tcp::socket socket_{io_context_};
    asio::streambuf buf_{};
    std::string address_{};
    std::uint16_t port_{};
    std::atomic<int> next_subscription_id_{};
    std::map<int, std::function<void(std::string_view)>> subscriptions_{};
};

} // namespace natsuki

#endif
