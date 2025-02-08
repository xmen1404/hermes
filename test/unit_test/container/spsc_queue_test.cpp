#include "hermes/container/spsc_queue.h"

#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>

#include "hermes/random/random.h"

using namespace hermes::container;
typedef hermes::random::IntegralRandom IRand;

TEST_CASE("Init Test") { SpscQueue<int, 1024> queue; }

TEST_CASE("Sequential Write Test") {
  constexpr uint32_t data_size = 1024;
  std::vector<int> data;

  SpscQueue<int, data_size> queue;
  for (auto i = 0; i < data_size; ++i) {
    const auto val = IRand::RandInt32(0, data_size);
    data.push_back(val);
    queue.Write(val);
  }

  REQUIRE(queue.SizeGuess() == data_size);
  REQUIRE(queue.IsEmpty() == false);

  auto idx = 0;
  while (!queue.IsEmpty()) {
    int val;
    REQUIRE(queue.Read(val));
    REQUIRE(data[idx] == val);
    idx += 1;
  }
}

TEST_CASE("Multi-thread Write/Read Test") {
  constexpr uint32_t data_size = 1e5;
  std::vector<int> data;

  for (uint32_t i = 0; i < data_size; ++i) {
    const auto val = IRand::RandInt32(0, data_size);
    data.push_back(val);
  }

  SpscQueue<int, data_size> queue;

  std::thread sender_th([&]() {
    for (uint32_t i = 0; i < data_size; ++i) {
      REQUIRE(queue.WriteAvailable());
      REQUIRE(queue.Write(data[i]));
    }
  });
  sender_th.join();

  std::thread receiver_th([&]() {
    uint32_t idx = 0;
    while (idx < data_size) {
      if (queue.ReadAvailable()) {
        int val;
        REQUIRE(queue.Read(val));
        REQUIRE(val == data[idx]);
        idx += 1;
      }
    }
  });
  receiver_th.join();
}
