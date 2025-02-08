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

  template <typename T>
  static T RandT(T left = std::numeric_limits<T>::min(),
                 T right = std::numeric_limits<T>::max()) noexcept {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return static_cast<T>(left + double(dis(gen)) * (right - left));
  }
};

}  // namespace hermes::random
