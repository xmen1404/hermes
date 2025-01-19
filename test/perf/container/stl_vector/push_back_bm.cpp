#include <benchmark/benchmark.h>

#include "hermes/container/stl_vector.h"

namespace bm = benchmark;

static void hermesVectorIntBM(bm::State &state) {
  const auto size = state.range(0);
  for (auto _ : state) {
    hermes::container::Vector<int> vi;

    for (auto i = 0; i < size; ++i)
      vi.PushBack(i);

    bm::DoNotOptimize(vi);
  }
}

static void stlVectorIntBM(bm::State &state) {
  const auto size = state.range(0);

  for (auto _ : state) {
    std::vector<int> vi;

    for (auto i = 0; i < size; ++i)
      vi.push_back(i);

    bm::DoNotOptimize(vi);
  }
}
BENCHMARK(stlVectorIntBM)->Range(0, 32768);
BENCHMARK(hermesVectorIntBM)->Range(0, 32768);

// run benchmark for struct with big data size

struct BigStruct {
  double d_[20];
  int32_t i_[20];
};

static void hermesVectorStructBM(bm::State &state) {
  const auto size = state.range(0);
  for (auto _ : state) {
    hermes::container::Vector<BigStruct> vi;

    for (auto i = 0; i < size; ++i)
      vi.PushBack(BigStruct{});

    bm::DoNotOptimize(vi);
  }
}

static void stlVectorStructBM(bm::State &state) {
  const auto size = state.range(0);

  for (auto _ : state) {
    std::vector<BigStruct> vi;

    for (auto i = 0; i < size; ++i)
      vi.push_back(BigStruct{});

    bm::DoNotOptimize(vi);
  }
}
BENCHMARK(stlVectorStructBM)->Range(0, 32768);
BENCHMARK(hermesVectorStructBM)->Range(0, 32768);

BENCHMARK_MAIN();
