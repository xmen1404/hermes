#include <benchmark/benchmark.h>

#include "hermes/container/spsc_queue.h"

namespace bm = benchmark;

static void hermesSpscIntBm(bm::State &state) {
  const auto size = state.range(0);
  for (auto _ : state) {
  }
}

BENCHMARK(hermesSpscIntBm)->Range(0, 10000);

BENCHMARK_MAIN();
