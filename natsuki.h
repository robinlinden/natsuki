#ifndef NATSUKI_H_
#define NATSUKI_H_

#include <asio.hpp>

#include <array>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace natsuki {

class Nats {
public:
    explicit Nats(std::string_view address) {
        asio::error_code ec;

        asio::ip::tcp::resolver resolver{io_service_};
        auto endpoints = resolver.resolve(address, "4222", ec);
        if (ec) { throw std::runtime_error(ec.message()); }

        asio::ip::tcp::socket socket{io_service_};
        asio::connect(socket, endpoints, ec);
        if (ec) { throw std::runtime_error(ec.message()); }

        while (true) {
            std::array<char, 512> buf;

            size_t len = socket.read_some(asio::buffer(buf), ec);
            if (ec == asio::error::eof) {
                break;
            } else if (ec) {
                throw std::runtime_error(ec.message());
            }

            std::cout.write(buf.data(), len);
            break;
        }
    }

private:
    asio::io_service io_service_{};
};

} // namespace natsuki

#endif
