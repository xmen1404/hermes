#include "hermes/container/optional.h"
#include <catch2/catch_test_macros.hpp>

using namespace hermes::container;

struct Tracker {
  static inline int instances = 0;
  Tracker() { ++instances; }
  Tracker(const Tracker &) { ++instances; }
  Tracker(Tracker &&) { ++instances; }
  ~Tracker() { --instances; }
};

TEST_CASE("Default construction") {
  Optional<int> opt;
  REQUIRE_FALSE(opt.HasValue());
}

TEST_CASE("Assignment and copy construction") {
  Optional<int> opt1;
  opt1 = 12;
  REQUIRE(opt1.HasValue());
  REQUIRE(opt1.Value() == 12);

  Optional<int> opt2(opt1);
  REQUIRE(opt2.HasValue());
  REQUIRE(opt2.Value() == 12);
}

TEST_CASE("Construction with parameters") {
  struct Data1 {
    int x, y;
    Data1(int a, int b) : x(a), y(b) {}
  };
  Optional<Data1> opt{1, 2};
  REQUIRE(opt.HasValue());
  REQUIRE(opt.Value().x == 1);
  REQUIRE(opt.Value().y == 2);
}

TEST_CASE("Assignment from a value") {
  Optional<int> opt;
  opt = 100;
  REQUIRE(opt.HasValue());
  REQUIRE(opt.Value() == 100);
}

TEST_CASE("Reset functionality") {
  Optional<int> opt;
  opt = 55;
  REQUIRE(opt.HasValue());
  opt.Reset();
  REQUIRE_FALSE(opt.HasValue());
}

TEST_CASE("Non-trivial type management (Tracker)") {
  int initial = Tracker::instances;
  {
    Optional<Tracker> opt;
    opt = Tracker();
    REQUIRE(opt.HasValue());
  }
  // After opt is destroyed, Tracker's destructor should run.
  REQUIRE(Tracker::instances == initial);
}

TEST_CASE("Conversion between different Optional types") {
  Optional<int> optInt;
  optInt = 42;
  // Convert Optional<int> to Optional<long>
  Optional<long> optLong(optInt);
  REQUIRE(optLong.HasValue());
  REQUIRE(optLong.Value() == 42);
}
