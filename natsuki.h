#ifndef NATSUKI_H_
#define NATSUKI_H_

#include <asio.hpp>

#include <array>
#include <atomic>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace natsuki {

class Nats {
public:
    explicit Nats(std::string address) : address_{std::move(address)} {}

    void run() {
        asio::error_code ec;

        asio::ip::tcp::resolver resolver{io_service_};
        auto endpoints = resolver.resolve(address_, "4222", ec);
        if (ec) { throw std::runtime_error(ec.message()); }

        asio::ip::tcp::socket socket{io_service_};
        asio::connect(socket, endpoints, ec);
        if (ec) { throw std::runtime_error(ec.message()); }

        while (running_.load()) {
            using namespace std::literals;

            std::array<char, 512> buf;

            size_t len = socket.read_some(asio::buffer(buf), ec);
            if (ec == asio::error::eof) {
                break;
            } else if (ec) {
                throw std::runtime_error(ec.message());
            }

            std::cout.write(buf.data(), len);

            if (len >= 6 && std::memcmp(buf.data(), "INFO", 4) == 0) {
                auto connect{"CONNECT {\"verbose\":true,\"pedantic\":true,\"name\":\"natsuki\",\"lang\":\"cpp\",\"version\":\"0.0.1\",\"protocol\":0}\r\n"sv};
                asio::write(socket, asio::buffer(connect));
            } else if (len == 6 && std::memcmp(buf.data(), "PING\r\n", 6) == 0) {
                asio::write(socket, asio::buffer("PONG\r\n"sv));
            }
        }
    }

    void shutdown() {
        running_ = false;
    }

private:
    asio::io_service io_service_{};
    std::string address_{};
    std::atomic<bool> running_{true};
};

} // namespace natsuki

#endif
