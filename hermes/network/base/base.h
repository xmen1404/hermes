#pragma once

#include <string>
#include <string_view>
#include <optional>

#include <glog/logging.h>

#include "hermes/network/zmq/zmq.hpp"
#include "hermes/config/config.h"

namespace hermes::network {

struct Message {
  char* data;
  size_t size;
};

class Socket {
public:
  Socket(int thread_num, zmq::socket_type type): context_{thread_num}, socket_{context_, type} {}    
  ~Socket() = default;

 public:
  virtual void Init(const hermes::config::Config& config) noexcept {
  }
  
  virtual bool Send(const Message& msg) noexcept {
    // TODO: optimize copy here
    // if send_bytes = 0, try again
    auto zmsg = zmq::message_t{msg.data, msg.size};
    auto ret = socket_.send(zmsg, zmq::send_flags::none);
    if (!ret.has_value() || !ret.value())
        return false;
    return true;
  }

  virtual std::optional<Message> Recv() noexcept {
    // TODO: optimize copy here
    auto msg = zmq::mutable_buffer{buffer_, MAX_BUFFER_SIZE};
    auto recv_size_opt = socket_.recv(msg);
    if (!recv_size_opt || !recv_size_opt.value().size)
      return {};
    return Message{buffer_, recv_size_opt.value().size};
  }

  void Connect(const std::string& addr) noexcept {
    socket_.connect(addr);
  }

  void Bind(const std::string& addr) noexcept {
    socket_.bind(addr);
  }

 public:
  zmq::context_t& GetContext() noexcept {
    return context_;
  }
  
  zmq::socket_t& GetSocket() noexcept {
    return socket_;
  }

 protected:
  static constexpr int MAX_BUFFER_SIZE = 1 << 20;

 protected:
  zmq::context_t context_;
  zmq::socket_t socket_;

  char buffer_[MAX_BUFFER_SIZE];
};

} // namespace hermes::network
