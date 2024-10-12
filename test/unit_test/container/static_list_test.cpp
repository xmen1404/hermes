#include <catch2/catch_test_macros.hpp>

#include "hermes/container/static_list.h"

using namespace hermes::container;

TEST_CASE("Init Test") {
  StaticList<int, 1024> list{};
  list.PushBack(12);
  REQUIRE(list.Size() == 1);
}

TEST_CASE("PushBack Delete") {
  int n = 6;
  int data[] = {1, 34, 545, 234, 23, 123};

  StaticList<int, 1024> list{};

  for (auto i = 0; i < n; ++i) {
    list.PushBack(data[i]);
  }
  list.PushBack(0);
  list.PushBack(1);
  list.Erase(list.End());
  list.Erase(list.End());

  for (auto i = 0; i < n; ++i) {
    REQUIRE(*list.Begin() == data[i]);
    list.Erase(list.Begin());
  }
}
