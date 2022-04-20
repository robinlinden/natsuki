// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#include "bench/natsc_subscriber.h"

#include <functional>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <nats.h>

namespace bench {
namespace {

void onMessage(natsConnection *, natsSubscription *, natsMsg *msg, void *userdata) {
    auto payload = std::string_view(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));
    (*reinterpret_cast<NatscSubscriber::CallbackT *>(userdata))(payload);
    natsMsg_Destroy(msg);
}

} // namespace

NatscSubscriber::NatscSubscriber(std::string address, std::string topic) : topic_{std::move(topic)} {
    natsStatus s = natsConnection_ConnectTo(&conn_, address.c_str());
    if (s != NATS_OK) {
        std::stringstream ss;
        ss << "Unable to connect to " << address << " (" << s << ')';
        throw std::runtime_error(ss.str());
    }
}

NatscSubscriber::~NatscSubscriber() {
    natsSubscription_Destroy(sub_);
    natsConnection_Destroy(conn_);
}

void NatscSubscriber::subscribe(std::function<void(std::string_view)> on_data) {
    // nats.c can call the callback from multiple threads at the same time.
    on_data_ = [inner = std::move(on_data), mtx = std::make_shared<std::mutex>()](auto data) mutable {
        std::scoped_lock lock{*mtx};
        inner(data);
    };

    natsStatus s = natsConnection_Subscribe(
            &sub_,
            conn_,
            topic_.c_str(),
            onMessage,
            reinterpret_cast<void *>(&on_data_));
    if (s != NATS_OK) {
        std::stringstream ss;
        ss << "Subscribe failed (" << s << ')';
        throw std::runtime_error(ss.str());
    }
}

} // namespace bench
