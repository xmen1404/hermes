#pragma once

#include <string>

#include "hermes/network/base/base.h"

namespace hermes::network {

class RouterSocket : public Socket {
public:
  RouterSocket(): Socket(1, zmq::socket_type::router) {
    LOG(INFO) << "Initializing ROUTER socket";
  }

public:
  void Init(const hermes::config::Config& config) noexcept override {
  }
};

} // namespace hermes::network

