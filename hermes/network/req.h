#pragma once

#include <string>

#include "hermes/network/base/base.h"

namespace hermes::network {

class ReqSocket : public Socket {
 public:
  ReqSocket() : Socket(1, zmq::socket_type::req) {}

 public:
  void Init(const hermes::config::Config &config) noexcept override {}
};

}  // namespace hermes::network
