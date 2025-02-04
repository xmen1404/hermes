#include <catch2/catch_test_macros.hpp>

#include "hermes/container/smart_pointer.h"

using namespace hermes::container;

TEST_CASE("Init Test") {
  auto uptr = UniquePtr<int>(new int(12));
  REQUIRE(*uptr.Get() == 12);
}

TEST_CASE("MakeUnique Test") {
  auto uptr = MakeUnique<int>(12);
  REQUIRE(*uptr.Get() == 12);
}

TEST_CASE("Transfer Ownership Test") {
  auto uptr = UniquePtr<int>(new int(12));
  auto n_ptr = std::move(uptr);
  REQUIRE(*n_ptr.Get() == 12);
}
