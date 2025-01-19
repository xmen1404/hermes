#include <gflags/gflags.h>
#include <glog/logging.h>
#include <string>
#include <vector>

#include "hermes/fastio/fastio.h"
#include "hermes/random/random.h"

signed main() {
  char data_path[] = "./test/data/input/integral_fastio_perf.input";
  const int MAX_BUFFER_SIZE = 1 << 2;

  hermes::fastio::IntegralFastIO<int64_t, MAX_BUFFER_SIZE> input;
  input.Init(data_path);

  // simply read all input
  int64_t value;
  while (input.IsGood()) {
    value = input.ReadOne();
  }
}
