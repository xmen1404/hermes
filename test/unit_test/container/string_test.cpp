#include "hermes/container/string.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>

using namespace hermes::container;

TEST_CASE("Init Test", "[String]") {
  String s{};
  REQUIRE(s.Size() == 0);
  s.PushBack('c');
  REQUIRE(s == String{"c"});
}

TEST_CASE("Construction from C-string literal", "[String]") {
  String s("Hello");
  REQUIRE(s.Size() == 5);
  REQUIRE(std::strcmp(s.CStr(), "Hello") == 0);
}

TEST_CASE("Construction with explicit size", "[String]") {
  const char *text = "World";
  String s(text, 5);
  REQUIRE(s.Size() == 5);
  REQUIRE(std::strcmp(s.CStr(), "World") == 0);
}

TEST_CASE("PushBack and PopBack", "[String]") {
  String s("abc");
  s.PushBack('d');
  REQUIRE(std::strcmp(s.CStr(), "abcd") == 0);
  s.PopBack();
  REQUIRE(std::strcmp(s.CStr(), "abc") == 0);
  // Popping on empty string should be safe.
  String empty;
  empty.PopBack();
  REQUIRE(empty.Size() == 0);
}

TEST_CASE("Equality operator", "[String]") {
  String s1("test");
  String s2("test");
  String s3("Test");
  REQUIRE(s1 == s2);
  REQUIRE(!(s1 == s3));
}

TEST_CASE("Copy constructor", "[String]") {
  String original("copy me");
  String copy(original);
  REQUIRE(copy == original);
  copy.PushBack('!');
  REQUIRE(!(copy == original));
}

TEST_CASE("Move constructor", "[String]") {
  String original("move me");
  size_t originalSize = original.Size();
  String moved(std::move(original));
  REQUIRE(moved.Size() == originalSize);
  // Moved-from object should be in a valid, empty state.
  REQUIRE(original.Size() == 0);
}

TEST_CASE("Copy assignment", "[String]") {
  String s1("assign me");
  String s2;
  s2 = s1;
  REQUIRE(s2 == s1);
}

TEST_CASE("Move assignment", "[String]") {
  String s1("move assign");
  String s2;
  s2 = std::move(s1);
  REQUIRE(s2.Size() > 0);
  // Moved-from object should be left in a valid state (empty).
  REQUIRE(s1.Size() == 0);
}

TEST_CASE("Element access via operator[]", "[String]") {
  String s("hello");
  REQUIRE(s[1] == 'e');
  s[1] = 'a';
  REQUIRE(std::strcmp(s.CStr(), "hallo") == 0);
}
