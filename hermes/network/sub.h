#pragma once

#include <string>

#include "hermes/network/base/base.h"

namespace hermes::network {

class SubSocket : public Socket {
public:
  SubSocket() : Socket(1, zmq::socket_type::sub) {
    LOG(INFO) << "Initializing SUB socket";
  }

public:
  void Init(const hermes::config::Config &config) noexcept override {}

  void AddTopic(const std::string &topic) noexcept {
    socket_.set(zmq::sockopt::subscribe, topic);
  }
};

} // namespace hermes::network
