#include <thread>
#include <chrono>

#include "hermes/network/rep.h"
#include "hermes/network/req.h"

using namespace hermes;

signed main() {
  const std::string server_addr = "tcp://0.0.0.0:5555";

  network::RepSocket server;
  server.Bind(server_addr);
 
  std::this_thread::sleep_for(std::chrono::seconds(1));

  network::ReqSocket client;
  client.Connect(server_addr);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  
  std::string data{"Hello"};
  client.Send(network::Message{data.data(), data.size()});
  
  auto msg_opt = server.Recv();
  if (msg_opt.has_value()) {
    auto msg = std::string_view{msg_opt->data, msg_opt->size};
    CHECK(msg == data) << "REP socket received wrong message expected: " << data << " but received: " << msg;
  }
  else {
    CHECK(false) << "REP socket failed to received the message";
  }
}

