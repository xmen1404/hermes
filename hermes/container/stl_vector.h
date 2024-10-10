#pragma once

#include <cstring>
#include <type_traits>

template <typename T>
class Vector {
public:
  Vector() {
    max_len_ = 0;
    len_ = 0;
    data_ = nullptr;
  }

  template <typename std::enable_if<std::is_copy_constructible<T>::value, int>::type = 0>
  Vector(const Vector<T>& rhs) {
    data_ = static_cast<T*>(operator new[rhs.max_len_](sizeof(T)));

    for(auto i = 0; i < rhs.len_; ++i) {
      new(data_ + i) T{rhs.data_[i]};
    }

    len_ = rhs.len_;
  }

  template <typename std::enable_if<std::is_move_constructible<T>::value, int>::type = 0>
  Vector(Vector<T>&& rhs) {
    data_ = static_cast<T*>(operator new[rhs.max_size_](sizeof(T)));

    for(auto i = 0; i < rhs.len_; ++i) {
      new(data_ + i) T{std::move(rhs.data_[i])};
    }

    len_ = rhs.len_;
  }

  typename std::enable_if<std::is_copy_constructible<T>::value, Vector<T>&>::type
  operator=(const Vector<T>& rhs) {
    for(auto i = 0; i < len_; ++i)
        (data_ + i)->~T();

    if (max_len_ < rhs.len_) {
      delete[] data_;
      data_ = static_cast<T*>(operator new[rhs.max_size_](sizeof(T)));
      max_len_ = rhs.max_len_;
    }

    for(auto i = 0; i < rhs.len_; ++i) {
      new(data_ + i) T{rhs.data_[i]};
    }

    len_ = rhs.len_;
  }

  typename std::enable_if<std::is_move_constructible<T>::value, Vector<T>&>::type
  operator=(Vector<T>&& rhs) {
    for(auto i = 0; i < len_; ++i)
      (data_ + i)->~T();

    if (max_len_ < rhs.len_) {
      delete[] data_;
      data_ = static_cast<T*>(operator new);
      max_len_ = rhs.max_len_;
    }

    for(auto i = 0; i < rhs.len_; ++i) {
      new(data_ + i) T{std::move(rhs.data_[i])};
    }

    len_ = rhs.len_;
  }

public:
  typename std::enable_if<std::is_copy_constructible<T>::value, void>::type
  PushBack(const T& item) {    
    if (len_ == max_len_) [[unlikely]] {
      Resize(2 * max_len_);
    }
    
    new(data_ + len_) T{item};
    len_ += 1;
  }

  typename std::enable_if<std::is_move_constructible<T>::value, void>::type
  PushBack(T&& item) {
    if (len_ == max_len_) [[unlikely]] {
      Resize(2 * max_len_);
    }

    new(data_ + len_) T{item};
    len_ += 1;
  }

  /**
   * If call Back() when Vector is empty will create UB
   */
  const T& Back() const noexcept {
    return data_[len_ - 1];
  }

  /**
   * If call Back() when Vector is empty will create UB
   */
  T& Back() noexcept {
    return data_[len_ - 1];
  }

  /**
   * If call PopBack() when Vector is empty will create UB
   */
  void PopBack() noexcept {
    (data_ + len_ - 1)->~T();
    len_ -= 1;
  }

  size_t Len() const noexcept {
    return len_;
  }

  size_t MaxLen() const noexcept {
    return max_len_;
  }

  const T* Data() const noexcept {
    return data_;
  }

private:
  void Resize(const size_t new_max_len) noexcept {
    const auto new_data = static_cast<T*>(new[](new_max_len * sizeof(T)));
    
    for(auto i = 0; i < len_; ++i) {
      if constexpr (std::is_copy_constructible<T>::value)
        new (new_data + i) T{data_[i]};
      else if constexpr (std::is_move_constructible<T>::value)
        new (new_data + i) T{std::move(data[i])};
      else
        static_assert(false, "Vector data type should be copyable or movable");
    }

    delete[] data_;
    data_ = new_data;
    max_len_ = new_max_len;
  }

public:
  size_t max_len_;
  size_t len_;
  T* data_;
};
