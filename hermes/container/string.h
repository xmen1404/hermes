#pragma once

#include <bit>
#include <cstring>
#include <type_traits>
#include <utility>

namespace hermes::container {

class StringView {
 public:
  char *Data() &noexcept { return 0; }

  const char *Data() const &noexcept { return 0; }

  size_t Size() const noexcept { return 0; }

 private:
};

class String {
 public:
  String() {}

  String(const String &rhs) { CopyFrom(rhs); }

  String(String &&rhs) { MoveFrom(std::move(rhs)); }

  explicit String(const char *s) { AssignValue(s, strlen(s)); }

  String(const char *s, const size_t size) { AssignValue(s, size); }

  ~String() noexcept { delete[] data_; }

  String &operator=(const String &rhs) {
    CopyFrom(rhs);
    return *this;
  }

  String &operator=(String &&rhs) noexcept {
    MoveFrom(std::move(rhs));
    return *this;
  }

  String &operator=(const StringView &sv) {
    AssignValue(sv.Data(), sv.Size());
    return *this;
  }

  bool operator==(const String &rhs) const noexcept {
    if (size_ != rhs.size_) return false;
    return std::memcmp(data_, rhs.data_, size_) == 0;
  }

 public:
  char *CStr() const noexcept { return data_; }

  size_t Size() const { return size_; }

  size_t MaxSize() const { return max_size_; }

  void PushBack(const char c) {
    const auto c_size = size_ + 1;
    if (c_size >= max_size_) [[unlikely]]
      Resize();
    data_[size_] = c;
    data_[size_ + 1] = '\0';
    size_ += 1;
  }

  void PopBack() {
    if (!size_) [[unlikely]]
      return;

    size_ -= 1;
    data_[size_] = '\0';
  }

  char &operator[](size_t idx) { return data_[idx]; }

 private:
  void Resize() {
    max_size_ = max_size_ ? (max_size_ << 1) : 2;
    auto *new_data = new char[max_size_];

    if (data_ != nullptr) {
      memcpy(new_data, data_, size_ + 1);
      delete[] data_;
    }

    data_ = new_data;
  }

  void CopyFrom(const String &rhs) {
    const auto c_size = rhs.size_ + 1;
    if (max_size_ < c_size) {
      delete[] data_;
      max_size_ = std::bit_ceil(c_size);
      data_ = new char[max_size_];
    }

    size_ = rhs.size_;
    memcpy(data_, rhs.data_, c_size);
  }

  void MoveFrom(String &&rhs) noexcept {
    std::swap(data_, rhs.data_);
    std::swap(size_, rhs.size_);
    std::swap(max_size_, rhs.max_size_);
  }

  void AssignValue(const char *ptr, size_t size) {
    const auto c_size = size + 1;
    if (max_size_ < c_size) {
      delete[] data_;
      max_size_ = std::bit_ceil(c_size);
      data_ = new char[max_size_];
    }

    size_ = size;
    memcpy(data_, ptr, c_size);
  }

 public:
  size_t size_{0};
  size_t max_size_{0};
  char *data_{nullptr};
};

}  // namespace hermes::container
