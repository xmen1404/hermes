#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include "hermes/container/static_list.h"
#include "hermes/random/random.h"

using namespace hermes::container;
typedef hermes::random::IntegralRandom IRand;

TEST_CASE("Init Test") {
  StaticList<int, 1024> list{};
  list.PushBack(12);
  REQUIRE(list.Size() == 1);
}

TEST_CASE("PushBack Delete Test") {
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

TEST_CASE("Big Data Size Test") {
  constexpr int OBJECT_SIZE = 1 << 12; // cacheline size
  constexpr int DATA_SIZE = 1024;

  struct Data {
    char data[OBJECT_SIZE];
  };

  StaticList<Data, DATA_SIZE> list{};
  for (auto i = 0; i < DATA_SIZE; ++i)
    list.PushBack(Data{});

  REQUIRE(list.Size() == 1024);
}

TEST_CASE("Random Insert Test") {
  constexpr int DATA_SIZE = 1e5 + 10;

  std::vector<std::pair<int, int>> data;
  for (auto i = 0; i < DATA_SIZE; ++i) {
    const auto insert_idx = hermes::random::IntegralRandom::RandInt32(0, i);
    data.push_back({insert_idx, i});
  }

  StaticList<int, DATA_SIZE> list{};

  for (const auto &p : data) {
    const auto val = p.second;
    const auto idx = p.first;

    auto it = list.Begin();

    for (auto step = 0; step < idx; ++step)
      it = std::next(it);
    list.Insert(it, val);
  }

  INFO("here" << data.size());

  REQUIRE(list.Size() == DATA_SIZE);

  std::vector<int> output;
  for (const auto &p : data) {
    const auto val = p.second;
    const auto idx = p.first;

    auto it = output.begin();
    for (auto step = 0; step < idx; ++step)
      it = std::next(it);
    output.insert(it, val);
  }

  auto it = list.Begin();
  for (auto i = 0; i < DATA_SIZE; ++i) {
    REQUIRE(output[i] == *it);
    it = std::next(it);
  }
}

TEST_CASE("PushFront; PushBack; Random Erase") {
  constexpr int DATA_SIZE = 1e5 + 10;

  StaticList<int, DATA_SIZE> list{};
  for (auto i = 0; i < DATA_SIZE; ++i) {
    if (i % 2)
      list.PushFront(i);
    else
      list.PushBack(i);
  }

  while (!list.IsEmpty()) {
    auto step = IRand::RandInt32(0, list.Size() - 1);
    auto it = list.Begin();
    for (auto i = 0; i < step; ++i) {
      it = std::next(it);
    }
    list.Erase(it);
  }

  REQUIRE(list.Size() == 0);
}
