#include <catch2/catch_test_macros.hpp>
#include <glog/logging.h>

#include "hermes/container/stl_shared_ptr.h"

using namespace hermes::container;

TEST_CASE("Init Test") {
  auto *data = new int{12};
  auto shared_ptr = SharedPtr<int>{data};
  REQUIRE(*shared_ptr.Get() == 12);
}

TEST_CASE("RefCnt Test") {
  int del_cnt = 0;

  class TestClass {
    int *del_cnt_;

  public:
    TestClass(int *ptr) : del_cnt_{ptr} {}
    ~TestClass() { (*del_cnt_)++; }
  };

  {
    auto obj_ptr = new TestClass(&del_cnt);
    auto ptr1 = SharedPtr<TestClass>(obj_ptr);
    REQUIRE(ptr1.IsUnique());

    auto ptr2 = ptr1;
    REQUIRE(!ptr1.IsUnique());
    SharedPtr<TestClass> ptr3{ptr2};
    {
      SharedPtr<TestClass> ptr4{ptr1};
      SharedPtr<TestClass> ptr5{ptr3};
      SharedPtr<TestClass> ptr6{std::move(ptr2)};
      REQUIRE(ptr6.Get() == ptr5.Get());
    }
  }

  REQUIRE(del_cnt == 1);
}
