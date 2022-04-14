// SPDX-FileCopyrightText: 2021-2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "natsuki/natsuki.h"

#include <asio.hpp>

#include <atomic>
#include <charconv>
#include <cstring>
#include <functional>
#include <iostream>
#include <istream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace natsuki {

void Nats::run() {
    std::vector<char> port(10, '\0');

    if (auto [ptr, ec] = std::to_chars(port.data(), port.data() + port.size() - 1, port_);
            ec != std::errc{}) {
        throw std::runtime_error(std::make_error_code(ec).message());
    }

    asio::error_code ec;

    asio::ip::tcp::resolver resolver{io_context_};
    auto endpoints = resolver.resolve(address_, std::string_view(port.data()), ec);
    if (ec) { throw std::runtime_error(ec.message()); }

    asio::connect(socket_, endpoints, ec);
    if (ec) { throw std::runtime_error(ec.message()); }

    asio::async_read_until(socket_, buf_, "\r\n", std::bind_front(&Nats::on_read, this));

    io_context_.run();
}

void Nats::shutdown() {
    io_context_.stop();
}

void Nats::publish(
        std::string_view subject,
        std::string_view payload,
        std::optional<std::string_view> reply_to) {
    std::stringstream ss;
    ss << "PUB " << subject << " ";
    if (reply_to) {
        ss << *reply_to << " ";
    }

    ss << payload.size() << "\r\n" << payload << "\r\n";
    asio::post(io_context_, [this, msg = ss.str()] {
        asio::write(socket_, asio::buffer(msg));
    });
}

Subscription Nats::subscribe(
        std::string_view subject,
        std::function<void(std::string_view)> cb,
        SubscriptionOptions opts) {
    std::stringstream ss;
    int sid = next_subscription_id_.fetch_add(1);
    ss << "SUB " << subject << " " << sid << "\r\n";

    if (opts.unsubscribe_after != SubscriptionOptions::kNever) {
        ss << "UNSUB " << sid << " " << opts.unsubscribe_after << "\r\n";
    }

    asio::post(io_context_, [this, sid, cb, msg = ss.str()] {
        subscriptions_[sid] = cb;
        asio::write(socket_, asio::buffer(msg));
    });

    return {sid};
}

std::future<void> Nats::unsubscribe(Subscription &&sid) {
    std::stringstream ss;
    ss << "UNSUB " << sid.id() << "\r\n";
    std::promise<void> unsubscribed;
    auto future = unsubscribed.get_future();

    asio::post(io_context_, [this, sid, unsubscribed = std::move(unsubscribed), msg = ss.str()]() mutable {
        subscriptions_.erase(sid.id());
        unsubscribed.set_value();
        asio::write(socket_, asio::buffer(msg));
    });

    return future;
}

void Nats::on_read(asio::error_code const &ec, std::size_t bytes_transferred) {
    using namespace std::literals;
    if (ec == asio::error::eof) {
        shutdown();
    } else if (ec) {
        throw std::runtime_error(ec.message());
    }

    std::string data{
            asio::buffers_begin(buf_.data()),
            asio::buffers_begin(buf_.data()) + bytes_transferred - std::strlen("\r\n")};
    buf_.consume(bytes_transferred);

    if (data.starts_with("MSG")) {
        // Parse out the subscription id.
        std::size_t sid_location = data.find_first_of(" ", strlen("MSG ")) + 1;
        int sid = -1;
        std::from_chars(data.data() + sid_location, data.data() + data.size(), sid);

        // Get the payload data.
        std::size_t payload_length = asio::read_until(socket_, buf_, "\r\n");
        std::string payload{
            asio::buffers_begin(buf_.data()),
            asio::buffers_begin(buf_.data()) + payload_length - std::strlen("\r\n")};
        buf_.consume(payload_length);

        if (subscriptions_.contains(sid)) {
            subscriptions_.at(sid)(payload);
        }
    } else if (data.starts_with("INFO")) {
        auto connect{"CONNECT {\"verbose\":false,\"pedantic\":true,\"name\":\"natsuki\",\"lang\":\"cpp\",\"version\":\"0.0.1\",\"protocol\":0}\r\n"sv};
        asio::write(socket_, asio::buffer(connect));
    } else if (data.starts_with("PING")) {
        asio::write(socket_, asio::buffer("PONG\r\n"sv));
    } else if (data.starts_with("+OK")) {
        // This is fine.
    } else {
        std::cerr << "Unhandled message: " << data << " (" << data.size() << ")\n";
    }

    asio::async_read_until(socket_, buf_, "\r\n", std::bind_front(&Nats::on_read, this));
}

} // namespace natsuki
