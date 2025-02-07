#include "hermes/container/any.h"
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <typeinfo>

using hermes::container::Any;
using hermes::container::AnyCast;

TEST_CASE("Default construction") {
  Any a;
  REQUIRE(a.HasValue() == false);
}

TEST_CASE("Type assignment") {
  Any a;
  a = 12;
  REQUIRE(a.Type() == typeid(int));

  a = std::string{"12"};
  REQUIRE(a.Type() == typeid(std::string));
}

TEST_CASE("Copy assignment") {
  Any a;
  Any b = std::string{"12"};
  a = b;
  REQUIRE(a.Type() == typeid(std::string));
  // Check that AnyCast returns the correct value.
  REQUIRE(AnyCast<std::string>(a) == "12");
}

TEST_CASE("Move assignment") {
  Any a;
  a = std::string("hello");
  Any b;
  b = std::move(a);
  REQUIRE(b.Type() == typeid(std::string));
  REQUIRE(AnyCast<std::string>(b) == "hello");
  // a should be empty after move.
  REQUIRE(a.HasValue() == false);
}

TEST_CASE("Copy construction") {
  Any a = 42;
  Any b(a);
  REQUIRE(b.Type() == typeid(int));
  REQUIRE(AnyCast<int>(b) == 42);
}

TEST_CASE("Move construction") {
  Any a = std::string("move me");
  Any b(std::move(a));
  REQUIRE(b.Type() == typeid(std::string));
  REQUIRE(AnyCast<std::string>(b) == "move me");
  // a should be empty after move.
  REQUIRE(a.HasValue() == false);
}

TEST_CASE("Reset functionality") {
  Any a = 3.14;
  REQUIRE(a.HasValue());
  a.Reset();
  REQUIRE(a.HasValue() == false);
}

TEST_CASE("AnyCast with rvalue Any") {
  Any a = std::string("rvalue test");
  auto s = AnyCast<std::string>(std::move(a));
  REQUIRE(s == "rvalue test");
  // After moving, the original Any should be empty.
  REQUIRE(a.HasValue() == false);
}
