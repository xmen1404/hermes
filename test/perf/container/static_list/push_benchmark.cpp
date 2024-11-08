#include <benchmark/benchmark.h>
#include <list>

#include "hermes/container/static_list.h"

namespace bm = benchmark;

template <int MAX_SIZE> static void hermesStaticListIntBM(bm::State &state) {
  const auto size = state.range(0);
  for (auto _ : state) {
    hermes::container::StaticList<int, MAX_SIZE> slist;

    for (auto i = 0; i < size; ++i)
      slist.PushBack(i);

    bm::DoNotOptimize(slist);
  }
}

static void stlListIntBM(bm::State &state) {
  const auto size = state.range(0);

  for (auto _ : state) {
    std::list<int> list;

    for (auto i = 0; i < size; ++i)
      list.push_back(i);

    bm::DoNotOptimize(list);
  }
}
BENCHMARK(stlListIntBM)->Arg(1 << 5);
BENCHMARK(hermesStaticListIntBM<1 << 5>)->Arg(1 << 5);

BENCHMARK(stlListIntBM)->Arg(1 << 10);
BENCHMARK(hermesStaticListIntBM<1 << 10>)->Arg(1 << 10);

BENCHMARK(stlListIntBM)->Arg(1 << 20);
BENCHMARK(hermesStaticListIntBM<1 << 20>)->Arg(1 << 20);

BENCHMARK_MAIN();
