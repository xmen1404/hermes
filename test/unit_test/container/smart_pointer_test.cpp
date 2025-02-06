#include <catch2/catch_test_macros.hpp>

#include "hermes/container/smart_pointer.h"

using namespace hermes::container;

TEST_CASE("Init Test", "[UniquePtr]") {
  auto uptr = UniquePtr<int>(new int(12));
  REQUIRE(*uptr.Get() == 12);
}

TEST_CASE("MakeUnique Test", "[UniquePtr]") {
  auto uptr = MakeUnique<int>(12);
  REQUIRE(*uptr.Get() == 12);
}

TEST_CASE("Transfer Ownership Test", "[UniquePtr]") {
  auto uptr = UniquePtr<int>(new int(12));
  auto n_ptr = std::move(uptr);
  REQUIRE(*n_ptr.Get() == 12);
}

TEST_CASE("Init Test", "[SharedPtr]") {
  auto *data = new int{12};
  auto shared_ptr = SharedPtr<int>{data};
  REQUIRE(*shared_ptr.Get() == 12);
}

TEST_CASE("RefCnt Test", "[SharedPtr]") {
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
    // Initially unique.
    REQUIRE(ptr1.IsUnique());

    // Copy increases ref count.
    auto ptr2 = ptr1;
    REQUIRE(!ptr1.IsUnique());

    SharedPtr<TestClass> ptr3{ptr2};
    {
      SharedPtr<TestClass> ptr4{ptr1};
      SharedPtr<TestClass> ptr5{ptr3};
      // Test move construction.
      SharedPtr<TestClass> ptr6{std::move(ptr2)};
      REQUIRE(ptr6.Get() == ptr5.Get());
    }
  }

  // After all SharedPtrs are destroyed, the object should be deleted exactly
  // once.
  REQUIRE(del_cnt == 1);
}

TEST_CASE("Reset Test", "[SharedPtr]") {
  int del_cnt = 0;

  class TestClass {
    int *del_cnt_;

  public:
    TestClass(int *ptr) : del_cnt_{ptr} {}
    ~TestClass() { (*del_cnt_)++; }
  };

  {
    // Create a SharedPtr and then reset it to release ownership.
    SharedPtr<TestClass> ptr(new TestClass(&del_cnt));
    REQUIRE(ptr.Get() != nullptr);
    ptr.Reset();
    // After Reset, the pointer should be null.
    REQUIRE(ptr.Get() == nullptr);
  }
  // The managed object should have been deleted.
  REQUIRE(del_cnt == 1);
}

TEST_CASE("Reset with new pointer Test", "[SharedPtr]") {
  int del_cnt = 0;

  class TestClass {
    int *del_cnt_;

  public:
    TestClass(int *ptr) : del_cnt_{ptr} {}
    ~TestClass() { (*del_cnt_)++; }
  };

  {
    // Create a SharedPtr managing one object.
    SharedPtr<TestClass> ptr(new TestClass(&del_cnt));
    // Now reset with a new pointer.
    ptr.Reset(new TestClass(&del_cnt));
    REQUIRE(ptr.Get() != nullptr);
    // The old object should have been deleted already.
    REQUIRE(del_cnt == 1);
  }
  // When ptr goes out of scope, the new object is deleted.
  // Total deletion count should now be 2.
  REQUIRE(del_cnt == 2);
}

TEST_CASE("Copy and Move Test", "[SharedPtr]") {
  int del_cnt = 0;

  class TestClass {
    int *del_cnt_;

  public:
    TestClass(int *ptr) : del_cnt_{ptr} {}
    ~TestClass() { (*del_cnt_)++; }
  };

  SharedPtr<TestClass> ptr1(new TestClass(&del_cnt));
  {
    // Test copy constructor.
    SharedPtr<TestClass> ptr2(ptr1);
    // Test move constructor.
    SharedPtr<TestClass> ptr3(std::move(ptr2));
    // Both ptr1 and ptr3 should manage the same object.
    REQUIRE(ptr1.Get() == ptr3.Get());
    // With at least two SharedPtr instances, the pointer is not unique.
    REQUIRE(!ptr1.IsUnique());
  }
  // After inner block, only ptr1 remains so it becomes unique.
  REQUIRE(ptr1.IsUnique());
  // When ptr1 goes out of scope, the object is deleted.
  // (Deletion count is verified in the next test block.)
}

TEST_CASE("Self-contained Lifetime Test", "[SharedPtr]") {
  int del_cnt = 0;

  class TestClass {
    int *del_cnt_;

  public:
    TestClass(int *ptr) : del_cnt_{ptr} {}
    ~TestClass() { (*del_cnt_)++; }
  };

  {
    SharedPtr<TestClass> ptr(new TestClass(&del_cnt));
    {
      auto ptr_copy = ptr;
      REQUIRE(ptr_copy.Get() == ptr.Get());
      REQUIRE(!ptr_copy.IsUnique());
    }
    // After inner block, only the original pointer remains.
    REQUIRE(ptr.IsUnique());
  }
  // When ptr goes out of scope, the object is deleted exactly once.
  REQUIRE(del_cnt == 1);
}

struct Dummy {
  int value;
  Dummy(int v) : value(v) {}
};

TEST_CASE("MakeShared creates and initializes object",
          "[SharedPtr][MakeShared]") {
  auto sp = MakeShared<Dummy>(42);

  // Check that the managed object is not null.
  REQUIRE(sp.Get() != nullptr);

  // Check that the object is correctly initialized.
  REQUIRE(sp.Get()->value == 42);

  // When first created, the shared pointer should be unique.
  REQUIRE(sp.IsUnique());
}

TEST_CASE("MakeShared deletes object when last SharedPtr goes out of scope",
          "[SharedPtr][MakeShared]") {
  int deletionCounter = 0;

  struct TestObject {
    int *deletionCounter;
    TestObject(int *dc) : deletionCounter(dc) {}
    ~TestObject() { (*deletionCounter)++; }
  };

  {
    auto sp = MakeShared<TestObject>(&deletionCounter);
    REQUIRE(sp.Get() != nullptr);
    REQUIRE(sp.IsUnique());

    {
      // Create a copy to increase the reference count.
      auto sp2 = sp;
      REQUIRE(!sp.IsUnique());
    }
    // After sp2 goes out of scope, sp should be unique again.
    REQUIRE(sp.IsUnique());
  }
  // After sp goes out of scope, the object should be deleted.
  REQUIRE(deletionCounter == 1);
}

TEST_CASE("Init Test", "[WeakPtr]") {
  auto *data = new int{12};
  auto shared_ptr = SharedPtr<int>{data};
  REQUIRE(shared_ptr.IsUnique());

  auto weak_ptr = WeakPtr<int>{shared_ptr};
  REQUIRE(weak_ptr.IsExpired() == false);
}

TEST_CASE("RefCnt Test", "[WeakPtr]") {
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
    // Initially unique.
    REQUIRE(ptr1.IsUnique());

    auto weak_ptr = WeakPtr<TestClass>{ptr1};
    auto ptr2 = weak_ptr.Lock();
    REQUIRE(!ptr2.IsUnique());
  }

  // After all SharedPtrs are destroyed, the object should be deleted exactly
  // once.
  REQUIRE(del_cnt == 1);
}
