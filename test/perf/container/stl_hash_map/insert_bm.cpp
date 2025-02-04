#include <benchmark/benchmark.h>

#include "hermes/container/stl_hash_map.h"

#include <memory>

namespace bm = benchmark;

static void hermesHashMapIntBM(bm::State &state) {
  const auto size = state.range(0);
  for (auto _ : state) {
    hermes::container::HashMap<int, int> mp;
    mp.Init();

    for (auto i = 0; i < size; ++i)
      mp.Insert({i, i});

    bm::DoNotOptimize(mp);
  }
}

static void stlHashMapIntBM(bm::State &state) {
  const auto size = state.range(0);

  for (auto _ : state) {
    std::unordered_map<int, int> mp;

    for (auto i = 0; i < size; ++i)
      mp.insert({i, i});

    bm::DoNotOptimize(mp);
  }
}

BENCHMARK(stlHashMapIntBM)->Range(0, 32768);
BENCHMARK(hermesHashMapIntBM)->Range(0, 32768);

// run benchmark for struct with big data size

struct BigStruct {
  double d_[20];
  int32_t i_[20];
};

static void hermesHashMapBigStructBM(bm::State &state) {
  const auto size = state.range(0);
  for (auto _ : state) {
    hermes::container::HashMap<int, BigStruct> mp;
    mp.Init();

    for (auto i = 0; i < size; ++i)
      mp.Insert({i, BigStruct{}});

    bm::DoNotOptimize(mp);
  }
}

static void stlHashMapBigStructBM(bm::State &state) {
  const auto size = state.range(0);

  for (auto _ : state) {
    std::unordered_map<int, BigStruct> mp;

    for (auto i = 0; i < size; ++i)
      mp.insert({i, BigStruct{}});

    bm::DoNotOptimize(mp);
  }
}
// BENCHMARK(stlHashMapBigStructBM)->Range(0, 32768);
// BENCHMARK(hermesHashMapBigStructBM)->Range(0, 32768);

BENCHMARK_MAIN();
