#pragma once

#include "hermes/container/base.h"

#include <atomic>
#include <cstring>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>

namespace hermes::container {

/**
 * Shared Pointer Class
 * @note:
 *  - Cannot create SharedPtr with empty content (nullptr for data)
 *  - Any empty object must not be used, otherwise will create OB
 */
template <typename T> class SharedPtr {
public:
  SharedPtr(T *data) : data_{data} {
    ref_cnt_ptr_ = static_cast<std::atomic<int> *>(operator new(
        CACHE_LINE_SIZE, std::align_val_t(CACHE_LINE_SIZE)));
    new (ref_cnt_ptr_) std::atomic<int>{1};
  }

  // User need to ensure original SharedPtr out-life this method execution
  // Otherwise, will create OB
  SharedPtr(const SharedPtr<T> &rhs) {
    rhs.ref_cnt_ptr_->fetch_add(1, std::memory_order_acq_rel);
    data_ = rhs.data_;
    ref_cnt_ptr_ = rhs.ref_cnt_ptr_;
  }

  SharedPtr(SharedPtr<T> &&rhs) {
    data_ = rhs.data_;
    ref_cnt_ptr_ = rhs.ref_cnt_ptr_;

    rhs.data_ = nullptr;
    rhs.ref_cnt_ptr_ = nullptr;
  }

  SharedPtr &operator=(const SharedPtr<T> &rhs) {
    // assume current shared_ptr is not empty
    auto cnt = ref_cnt_ptr_->fetch_sub(1, std::memory_order_acq_rel);
    if (cnt == 1) {
      delete data_;
      ref_cnt_ptr_->~atomic();
      operator delete(ref_cnt_ptr_);
    }

    rhs.ref_cnt_ptr_->fetch_add(1, std::memory_order_acq_rel);
    data_ = rhs.data_;
    ref_cnt_ptr_ = rhs.ref_cnt_ptr_;
  }

  SharedPtr &operator=(SharedPtr<T> &&rhs) {
    // assume current shared_ptr is not empty
    auto cnt = ref_cnt_ptr_->fetch_sub(1, std::memory_order_acq_rel);
    if (cnt == 1) {
      delete data_;
      ref_cnt_ptr_->~atomic();
      operator delete(ref_cnt_ptr_);
    }

    data_ = rhs.data_;
    ref_cnt_ptr_ = rhs.ref_cnt_ptr_;

    rhs.data_ = nullptr;
    rhs.ref_cnt_ptr_ = nullptr;
  }

  ~SharedPtr() {
    if (!ref_cnt_ptr_)
      return;
    auto cnt = ref_cnt_ptr_->fetch_sub(1, std::memory_order_acq_rel);
    if (cnt == 1) {
      delete data_;
      ref_cnt_ptr_->~atomic();
      operator delete(ref_cnt_ptr_);
    }
  }

public:
  T *Get() { return data_; }

  bool IsUnique() const {
    return ref_cnt_ptr_->load(std::memory_order_acquire) == 1;
  }

private:
  static constexpr size_t CACHE_LINE_SIZE = 64;

public:
  T *data_;
  std::atomic<int> *ref_cnt_ptr_;
};

} // namespace hermes::container
