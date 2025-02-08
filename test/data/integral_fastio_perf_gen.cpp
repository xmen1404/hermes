#include <fstream>
#include <iostream>
#include <string>

#include "hermes/random/random.h"

signed main() {
  const std::string FILENAME = "integral_fastio_perf.input";
  const int test_size = 1e8;

  std::ofstream output(FILENAME);
  for (int i = 0; i < test_size; ++i) {
    output << hermes::random::IntegralRandom::RandInt64(0) << '\n';
  }

  output.close();
}
