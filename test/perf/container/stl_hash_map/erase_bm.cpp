#include <benchmark/benchmark.h>

#include "hermes/container/stl_hash_map.h"
#include "hermes/random/random.h"

namespace bm = benchmark;

static void hermesHashMapIntBM(bm::State &state) {
  const auto size = state.range(0);
  hermes::container::HashMap<int, int> mp;
  mp.Init();

  for (auto i = 0; i < size; ++i) mp.Insert({i, i});

  for (auto _ : state) {
    auto id = hermes::random::IntegralRandom::RandT<int>(0, size - 1);
    auto it = mp.Find(id);
    if (it != mp.End()) mp.Erase(it);
  }
}

static void stlHashMapIntBM(bm::State &state) {
  const auto size = state.range(0);

  std::unordered_map<int, int> mp;
  for (auto i = 0; i < size; ++i) {
    mp.insert({i, i});
  }

  for (auto _ : state) {
    auto id = hermes::random::IntegralRandom::RandT<int>(0, size - 1);
    auto it = mp.find(id);
    if (it != mp.end()) mp.erase(it);
  }
}

BENCHMARK(stlHashMapIntBM)->Range(0, 32768);
BENCHMARK(hermesHashMapIntBM)->Range(0, 32768);

// run benchmark for struct with big data size

BENCHMARK_MAIN();
