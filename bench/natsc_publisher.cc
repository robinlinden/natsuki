// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "bench/natsc_publisher.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <nats.h>

namespace bench {

NatscPublisher::NatscPublisher(std::string address, std::string topic) : topic_{std::move(topic)} {
    natsStatus s = natsConnection_ConnectTo(&conn_, address.c_str());
    if (s != NATS_OK) {
        std::stringstream ss;
        ss << "Unable to connect to " << address << " (" << s << ')';
        throw std::runtime_error(ss.str());
    }
}

NatscPublisher::~NatscPublisher() {
    natsConnection_Destroy(conn_);
}

void NatscPublisher::publish(std::string_view data) {
    natsMsg *msg{nullptr};
    natsStatus s = natsMsg_Create(
            &msg,
            topic_.c_str(),
            nullptr,
            data.data(),
            static_cast<int>(data.length()));
    if (s != NATS_OK) {
        std::stringstream ss;
        ss << "Unable to create message (" << s << ')';
        throw std::runtime_error(ss.str());
    }

    s = natsConnection_PublishMsg(conn_, msg);
    natsMsg_Destroy(msg);
    if (s != NATS_OK) {
        std::stringstream ss;
        ss << "Unable to publish message (" << s << ')';
        throw std::runtime_error(ss.str());
    }
}

} // namespace bench
