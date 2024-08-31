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

  network::DealerSocket back;
  back.Bind(back_addr);

  network::RouterSocket front;
  front.Bind(front_addr);

  network::ReqSocket client;
  client.Connect(front_addr);

  network::RepSocket server;
  server.Connect(back_addr);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::string data{"Hello"};
  client.Send(network::Message{data.data(), data.size()});

  for(auto i = 0; i < 3; ++i) {
    auto msg_opt = front.Recv();
    if (msg_opt.has_value()) {
      auto msg = std::string_view{msg_opt->data, msg_opt->size};
      LOG(INFO) << msg;
    }
  }
 
}

