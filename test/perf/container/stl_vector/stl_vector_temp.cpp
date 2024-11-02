#include <random>
#include <vector>

#include "hermes/container/stl_vector.h"

signed main() {
  // hermes::container::Vector<int> vi;
  std::vector<int> vi;

  const int size = 1 << 15;
  for (auto i = 0; i < size; ++i) {
    vi.push_back(i);
  }

  for (auto i = 0; i < size; ++i) {
  }
}
