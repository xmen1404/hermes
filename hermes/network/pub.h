#pragma once

#include <string>

#include "hermes/network/base/base.h"

namespace hermes::network {

class PubSocket : public Socket {
 public:
  PubSocket() : Socket(1, zmq::socket_type::pub) {
    LOG(INFO) << "Initializing PUB socket";
  }

 public:
  void Init(const hermes::config::Config &config) noexcept override {
    // implement me
  }
};

}  // namespace hermes::network
