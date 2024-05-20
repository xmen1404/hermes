#include "hermes/fastio/fastio.h"
#include <iostream>

int main() {
  std::FILE* fp = std::fopen("test/data/integral_fastio_test.input", "r");
  hermes::fastio::IntegralFastIO<int> input(fp);
  
  int value;
  while (input.ReadOne(value)) {
    std::cout << value << '\n';
  }
}
