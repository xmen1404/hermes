#include "hermes/container/stl_hash_map.h"

#include <catch2/catch_test_macros.hpp>

using namespace hermes::container;

TEST_CASE("Init Test") {
  auto mp = HashMap<int, int>();
  mp.Init();

  REQUIRE(mp.Insert({12, 12}).second == true);
  REQUIRE(mp[12] == 12);
}

TEST_CASE("High Load Test") {
  auto mp = HashMap<int, int>();
  mp.Init();

  const int k_data_size = 1e2;

  for (auto i = 0; i < k_data_size; ++i) {
    mp.Insert({i, i + 1});
  }

  REQUIRE(mp.Size() == k_data_size);
  for (auto i = 12; i < k_data_size; ++i) {
    REQUIRE(mp[i] == i + 1);
  }
}
