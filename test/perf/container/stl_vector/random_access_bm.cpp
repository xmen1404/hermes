#include <benchmark/benchmark.h>

#include <random>

#include "hermes/container/stl_vector.h"

namespace bm = benchmark;

typedef std::mt19937 rng_type;

static void BM_hermes_vector(bm::State &state) {
  const auto size = state.range(0);

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, size - 1);

  std::vector<int> access_index;
  for (auto i = 0; i < size; ++i) {
    access_index.push_back(dist(rng));
  }

  hermes::container::Vector<int> vi;
  for (auto i = 0; i < size; ++i) vi.PushBack(i);

  for (auto _ : state) {
    for (auto i = 0; i < size; ++i) {
      auto x = vi[access_index[i]];
      bm::DoNotOptimize(x);
    }
  }
}

static void BM_stl_vector(bm::State &state) {
  const auto size = state.range(0);

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, size - 1);

  std::vector<int> access_index;
  for (auto i = 0; i < size; ++i) {
    access_index.push_back(dist(rng));
  }

  std::vector<int> vi;
  for (auto i = 0; i < size; ++i) vi.push_back(i);

  for (auto _ : state) {
    for (auto i = 0; i < size; ++i) {
      auto x = vi[access_index[i]];
      bm::DoNotOptimize(x);
    }
  }
}

BENCHMARK(BM_hermes_vector)->Range(0, 32768 * 4);
BENCHMARK(BM_stl_vector)->Range(0, 32768 * 4);
