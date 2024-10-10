#include <thread>
#include <chrono>

#include "hermes/network/rep.h"
#include "hermes/network/req.h"
#include "hermes/network/dealer.h"
#include "hermes/network/router.h"

using namespace hermes;

// TODO: apply catch2 lib for testing



signed main() {
  const std::string front_addr = "tcp://0.0.0.0:5001";
  const std::string back_addr = "tcp://0.0.0.0:5002";

  network::RouterSocket front;
  front.Bind(front_addr);

  network::ReqSocket client;
  client.Connect(front_addr);

  network::RepSocket server;
  server.Bind(back_addr);

  network::DealerSocket back;
  back.Connect(back_addr);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::string data{"Hello"};
  client.Send(network::Message{data.data(), data.size()});

  std::string id_frame{"IdNumber"};
  back.Send(network::Message{id_frame.data(), id_frame.size()});

  std::string empty_frame{""};
  back.Send(network::Message{id_frame.data(), id_frame.size()});

  std::this_thread::sleep_for(std::chrono::seconds(1)); 

  // front receive message
  {
    for(auto i = 0; i < 3; ++i) {
      auto msg_opt = front.Recv();
      if (msg_opt.has_value()) {
        auto msg = std::string_view{msg_opt->data, msg_opt->size};
        LOG(INFO) << "FRONT received message: " << msg;
        back.Send(msg_opt.value(), i < 2);
      }
      else {
        LOG(INFO) << "FRONT received empty message";
        back.Send(network::Message{empty_frame.data(), empty_frame.size()}, true);
      }
    }
  }

 std::this_thread::sleep_for(std::chrono::seconds(1));

  // server receive message
  {
    auto msg_opt = server.Recv();
    if (msg_opt.has_value()) {
      auto msg = std::string_view{msg_opt->data, msg_opt->size};
      LOG(INFO) << "SERVER received message: " << msg;
    }
  }
 
}

