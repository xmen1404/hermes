#include <benchmark/benchmark.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "hermes/io/fastio.h"
#include "hermes/random/random.h"

namespace bm = benchmark;
namespace rnd = hermes::random;

void GenerateData(const std::string &filename, const int data_size) {
  std::ofstream outfile(filename, std::ios::out | std::ios::trunc);

  for (auto i = 0; i < data_size; ++i) {
    outfile << rnd::IntegralRandom::RandT<uint64_t>() << ' ';
  }

  outfile.flush();
  outfile.close();
}

static void hermesIFIntBM(bm::State &state) {
  const int data_size = state.range(0);
  const auto filename = "data.txt";
  GenerateData(filename, data_size);

  const int MAX_BUFFER_SIZE = 1 << 16;
  for (auto _ : state) {
    hermes::io::IntegralFastIO<uint64_t, MAX_BUFFER_SIZE> reader;
    reader.Init(filename);

    while (reader.IsGood()) {
      bm::DoNotOptimize(reader.ReadOne());
    }

    bm::DoNotOptimize(reader);
  }

  std::filesystem::remove(filename);
}

BENCHMARK(hermesIFIntBM)->Arg(1e5);
// BENCHMARK(hermesStaticListIntBM<1 << 5>)->Arg(1 << 5);

BENCHMARK_MAIN();
