#include <chrono>
#include <thread>

#include "hermes/network/pub.h"
#include "hermes/network/sub.h"

using namespace hermes;

signed main() {
  const std::string pub_addr = "tcp://0.0.0.0:5555";

  network::PubSocket pub;
  pub.Bind(pub_addr);

  network::SubSocket sub;
  sub.Connect(pub_addr);

  const std::string topic = "topicA";
  sub.AddTopic("");
  // sub.AddTopic(topic);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::string data{"topicA Hello"};
  pub.Send(network::Message{data.data(), data.size()});

  auto msg_opt = sub.Recv();
  if (msg_opt.has_value()) {
    auto msg = std::string_view{msg_opt->data, msg_opt->size};
    LOG(INFO) << "Received Message: " << msg;
  }
}
