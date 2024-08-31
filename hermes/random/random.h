#pragma once

#include <limits>
#include <random>

namespace hermes::random {

class IntegralRandom {
public:
  static bool RandBool() noexcept {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);     
    return d(gen);
  }

  static int
  RandInt32(int left = std::numeric_limits<int32_t>::min(),
            int right = std::numeric_limits<int32_t>::max()) noexcept {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(left, right);
    return distrib(gen);
  }

  static int64_t
  RandInt64(int64_t left = std::numeric_limits<int64_t>::min(),
            int64_t right = std::numeric_limits<int64_t>::max()) noexcept {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return int64_t(left + double(dis(gen)) * (right - left));
  }
};

} // namespace hermes::random
