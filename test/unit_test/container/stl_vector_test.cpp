#include <catch2/catch_test_macros.hpp>

#include "hermes/container/stl_vector.h"

using namespace hermes::container;

TEST_CASE("Init Test") {
  auto vi = Vector<int>();
  vi.PushBack(12);
  REQUIRE(vi.Back() == 12);
}

TEST_CASE("PushBack PopBack Test") {
  const int n = 8;
  const int data[] = {1, 9, 12, 534, 234, 23, 23, 345};

  auto vi = Vector<int>();
  for (auto i = 0; i < n; ++i) {
    vi.PushBack(data[i]);
  }

  for (auto i = 0; i < n; ++i) {
    REQUIRE(vi[i] == data[i]);
  }

  for (int i = 0; i < n - 1; ++i)
    vi.PopBack();

  REQUIRE(vi.Back() == data[0]);
}
