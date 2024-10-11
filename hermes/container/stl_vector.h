#pragma once

#include <cstring>
#include <type_traits>

namespace hermes::container {

template <typename T>
class Vector {
public:
  Vector() {
    max_size_ = 0;
    size_ = 0;
    data_ = nullptr;
  }

  template <typename std::enable_if<std::is_copy_constructible<T>::value, int>::type = 0>
  Vector(const Vector<T>& rhs) {
    data_ = static_cast<T*>(operator new[](rhs.max_size_ * sizeof(T)));

    for(auto i = 0; i < rhs.size_; ++i) {
      new(data_ + i) T{rhs.data_[i]};
    }

    size_ = rhs.size_;
  }

  template <typename std::enable_if<std::is_move_constructible<T>::value, int>::type = 0>
  Vector(Vector<T>&& rhs) {
    data_ = static_cast<T*>(operator new[](rhs.max_size_ * sizeof(T)));

    for(auto i = 0; i < rhs.size_; ++i) {
      new(data_ + i) T{std::move(rhs.data_[i])};
    }

    size_ = rhs.size_;
  }

  ~Vector() {
    operator delete[](data_);
  }

  typename std::enable_if<std::is_copy_constructible<T>::value, Vector<T>&>::type
  operator=(const Vector<T>& rhs) {
    for(auto i = 0; i < size_; ++i)
        (data_ + i)->~T();

    if (max_size_ < rhs.size_) {
      operator delete[](data_);
      data_ = static_cast<T*>(operator new[](rhs.max_size_ * sizeof(T)));
      max_size_ = rhs.max_size_;
    }

    for(auto i = 0; i < rhs.size_; ++i) {
      new(data_ + i) T{rhs.data_[i]};
    }

    size_ = rhs.size_;
  }

  typename std::enable_if<std::is_move_constructible<T>::value, Vector<T>&>::type
  operator=(Vector<T>&& rhs) {
    for(auto i = 0; i < size_; ++i)
      (data_ + i)->~T();

    if (max_size_ < rhs.size_) {
      operator delete[] (data_);
      data_ = static_cast<T*>(operator new[](rhs.max_size_ * sizeof(T)));
      max_size_ = rhs.max_size_;
    }

    for(auto i = 0; i < rhs.size_; ++i) {
      new(data_ + i) T{std::move(rhs.data_[i])};
    }

    size_ = rhs.size_;
  }

public:
  typename std::enable_if<std::is_copy_constructible<T>::value, void>::type
  PushBack(const T& item) {    
    if (size_ == max_size_) [[unlikely]] {
      Resize();
    }
    
    new(data_ + size_) T{item};
    size_ += 1;
  }

  typename std::enable_if<std::is_move_constructible<T>::value, void>::type
  PushBack(T&& item) {
    if (size_ == max_size_) [[unlikely]] {
      Resize();
    }

    new(data_ + size_) T{item};
    size_ += 1;
  }

  const T& operator[](size_t index) const noexcept {
    return data_[index];
  }

  T& operator[](size_t index) noexcept {
    return data_[index];
  }

  /**
   * If call Back() when Vector is empty will create UB
   */
  const T& Back() const noexcept {
    return data_[size_ - 1];
  }

  /**
   * If call Back() when Vector is empty will create UB
   */
  T& Back() noexcept {
    return data_[size_ - 1];
  }

  /**
   * If call PopBack() when Vector is empty will create UB
   */
  void PopBack() noexcept {
    (data_ + size_ - 1)->~T();
    size_ -= 1;
  }

  size_t size() const noexcept {
    return size_;
  }

  size_t MaxSize() const noexcept {
    return max_size_;
  }

  const T* Data() const noexcept {
    return data_;
  }

private:
  void Resize() noexcept {
    const auto new_max_size = !max_size_ ? 1 : 2 * max_size_;
    const auto new_data = static_cast<T*>(operator new[](new_max_size * sizeof(T)));
    
    static_assert(std::is_copy_constructible<T>::value | std::is_move_constructible<T>::value,  "Vector data type should be copyable or movable");

    for(size_t i = 0; i < size_; ++i) {
      if constexpr (std::is_move_constructible<T>::value)
        new (new_data + i) T{std::move(data_[i])};
      else
        new (new_data + i) T{data_[i]};
      
      data_[i].~T();
    }

    operator delete[](data_);
    data_ = new_data;
    max_size_ = new_max_size;
  }

public:
  size_t max_size_;
  size_t size_;
  T* data_;
};

} // namespace hermes::container

