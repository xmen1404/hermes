#pragma once

#include "hermes/network/base/base.h"

namespace hermes::network {

class DealerSocket : public Socket {
public:
  DealerSocket(): Socket(1, zmq::socket_type::dealer) {
    LOG(INFO) << "Initializing DEALER socket";
  }

public:
  void Init(const hermes::config::Config& config) noexcept override {
  }
};

} // namespace network

