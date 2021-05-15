#ifndef NATSUKI_H_
#define NATSUKI_H_

#include <asio.hpp>

#include <cstring>
#include <functional>
#include <iostream>
#include <istream>
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

        asio::connect(socket_, endpoints, ec);
        if (ec) { throw std::runtime_error(ec.message()); }

        asio::async_read_until(socket_, buf_, "\r\n",
                std::bind(&Nats::on_read, this, std::placeholders::_1, std::placeholders::_2));

        io_service_.run();
    }

    void shutdown() {
        io_service_.stop();
    }

private:
    void on_read(asio::error_code const &ec, std::size_t bytes_transferred) {
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
        std::cout << data << std::endl;

        if (data.starts_with("INFO")) {
            auto connect{"CONNECT {\"verbose\":true,\"pedantic\":true,\"name\":\"natsuki\",\"lang\":\"cpp\",\"version\":\"0.0.1\",\"protocol\":0}\r\n"sv};
            asio::write(socket_, asio::buffer(connect));
        } else if (data.starts_with("PING")) {
            asio::write(socket_, asio::buffer("PONG\r\n"sv));
        } else if (data.starts_with("+OK")) {
            // This is fine.
        } else {
            std::cerr << "Unhandled message: " << data << " (" << data.size() << ")\n";
        }

        asio::async_read_until(socket_, buf_, "\r\n",
                std::bind(&Nats::on_read, this, std::placeholders::_1, std::placeholders::_2));
    }

    asio::io_service io_service_{};
    asio::ip::tcp::socket socket_{io_service_};
    asio::streambuf buf_{};
    std::string address_{};
};

} // namespace natsuki

#endif
