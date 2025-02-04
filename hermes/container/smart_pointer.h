#pragma once

#include <glog/logging.h>

namespace hermes::container {

template <typename T> class UniquePtr {
public:
  explicit UniquePtr(T *ptr = nullptr) { data_ = ptr; }

  UniquePtr(UniquePtr &&rhs) : data_{rhs.data_} { rhs.data_ = nullptr; }

  ~UniquePtr() noexcept { delete data_; }

  UniquePtr &operator=(UniquePtr &rhs) = delete;
  UniquePtr &operator=(const UniquePtr &rhs) = delete;
  UniquePtr &operator=(UniquePtr &&rhs) { std::swap(data_, rhs.data_); }

public:
  T *Get() const noexcept { return data_; }

  explicit operator bool() const noexcept { return data_ != nullptr; }

private:
  T *data_;
};

template <typename T, typename... Args>
inline UniquePtr<T> MakeUnique(Args &&...params) {
  return UniquePtr<T>{new T{std::forward<decltype(params)>(params)...}};
}

} // namespace hermes::container
