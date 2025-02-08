#pragma once

#include <cstring>
#include <type_traits>

namespace hermes::container {

// TODO: finish the implementation
class String {
 public:
  String() : size_{0}, max_size_{0}, data_{nullptr} {}

  String(const String &rhs) {
    size_ = rhs.size_;
    max_size_ = rhs.max_size_;
    data_ = new char[max_size_];
    std::memcpy(data_, rhs.data_, size_ + 1);
  }

  String(String &&rhs) {
    size_ = rhs.size_;
    max_size_ = rhs.max_size_;
    data_ = rhs.data_;
    rhs.data_ = nullptr;
  }

  String(const char *s) {
    size_ = strlen(s);
    max_size_ = 1;
    while (max_size_ < size_ + 1) max_size_ <<= 1;
    data_ = new char[max_size_];
    std::memcpy(data_, s, size_);
  }

  String(const char *s, const size_t size) {
    size_ = size;
    max_size_ = 1;
    while (max_size_ < size_ + 1) max_size_ <<= 1;
    data_ = new char[max_size_];
    std::memcpy(data_, s, size_);
  }

  ~String() { delete[] data_; }

 public:
  size_t Size() const { return size_; }

  size_t MaxSize() const { return max_size_; }

  void PushBack(const char c) {
    if ((size_ + 1) == max_size_) [[unlikely]]
      Resize();
    data_[size_] = c;
    size_ += 1;
  }

  void PopBack() {
    if (!size_) [[unlikely]]
      return;

    size_ -= 1;
  }

  char &operator[](size_t idx) {
    CHECK(idx < size_) << "Access index out of range";
    return data_[idx];
  }

  const char &operator[](size_t idx) {
    CHECK(idx < size_) << "Access index out of range";
    return data_[idx];
  }

 private:
  void Resize() {
    max_size_ = max_size_ ? (max_size_ << 1) : 1;
    auto *new_data = new char[max_size_];
    std::memcpy(new_data, data_, size_);
    delete[] data_;
    data_ = new_data;
  }

 public:
  size_t size_;
  size_t max_size_;
  char *data_;
};

}  // namespace hermes::container
