#pragma once

#include <string>

#include "hermes/network/base/base.h"

namespace hermes::network {

class RepSocket : public Socket {
public:
  RepSocket() : Socket(1, zmq::socket_type::rep) {
    LOG(INFO) << "Initializing REP socket";
  }

public:
  void Init(const hermes::config::Config &config) noexcept override {}
};

} // namespace hermes::network
