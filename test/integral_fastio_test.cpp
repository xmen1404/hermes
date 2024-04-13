#include "hermes/fastio/fast_io.h"
#include <iostream>

int main() {
  std::FILE* fp = std::fopen("test/data/integral_fastio_input.txt", "r");
  hermes::fastio::IntegerFastIO<int> a(stdin);
  
  int value;
  while (a.ReadOne(value)) {
    std::cout << value << '\n';
  }
}
