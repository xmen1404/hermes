#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <glog/logging.h>

#include "hermes/config/config.h"
#include "hermes/network/zmq/zmq.hpp"

namespace hermes::network {

struct Message {
  char *data;
  size_t size;
};

class Socket {
public:
  Socket(int thread_num, zmq::socket_type type)
      : context_{thread_num}, socket_{context_, type} {}
  ~Socket() = default;

public:
  virtual void Init(const hermes::config::Config &config) noexcept {}

  virtual bool Send(const Message &msg, const bool send_more = false) noexcept {
    // TODO: optimize copy here
    // if send_bytes = 0, try again
    auto send_flags =
        zmq::send_flags::dontwait |
        (send_more ? zmq::send_flags::sndmore : zmq::send_flags::none);
    auto zmsg = zmq::message_t{msg.data, msg.size};
    auto ret = socket_.send(zmsg, send_flags);
    return ret.has_value();
  }

  virtual std::optional<Message> Recv() noexcept {
    // TODO: optimize copy here
    auto msg = zmq::mutable_buffer{buffer_, MAX_BUFFER_SIZE};
    auto recv_size_opt = socket_.recv(msg, zmq::recv_flags::dontwait);
    if (!recv_size_opt || !recv_size_opt.value().size)
      return {};
    return Message{buffer_, recv_size_opt.value().size};
  }

  void Connect(const std::string &addr) noexcept { socket_.connect(addr); }

  void Bind(const std::string &addr) noexcept { socket_.bind(addr); }

public:
  zmq::context_t &GetContext() noexcept { return context_; }

  zmq::socket_t &GetSocket() noexcept { return socket_; }

protected:
  static constexpr int MAX_BUFFER_SIZE = 1 << 20;

protected:
  zmq::context_t context_;
  zmq::socket_t socket_;

  char buffer_[MAX_BUFFER_SIZE];
};

} // namespace hermes::network
