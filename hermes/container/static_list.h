#pragma once

#include <cstring>
#include <iterator>
#include <type_traits>
#include <utility>

namespace hermes::container {

template <typename T, int MAX_SIZE> class StaticListAllocator {
public:
  StaticListAllocator() {
    data_ = static_cast<T *>(operator new[](MAX_SIZE * sizeof(T)));
    size_ = 0;

    free_cnt_ = MAX_SIZE;
    free_list_ = new int[MAX_SIZE];
    for (auto i = 0; i < free_cnt_; ++i)
      free_list_[i] = i;
  }

  ~StaticListAllocator() {
    operator delete[](data_);
    delete[] free_list_;
  }

public:
  T *New() noexcept {
    if (!free_cnt_) [[unlikely]]
      return nullptr;

    free_cnt_ -= 1;
    return data_ + free_list_[free_cnt_];
  }

  bool Delete(T *ptr) noexcept {
    auto diff = reinterpret_cast<size_t>(ptr) - reinterpret_cast<size_t>(data_);
    if (diff < 0 || diff % sizeof(T) || free_cnt_ == MAX_SIZE) [[unlikely]]
      return false;

    free_list_[free_cnt_] = ptr - data_;

    return true;
  }

private:
  size_t size_;
  T *data_;

  int *free_list_;
  int free_cnt_;
};

// TODO(@vspm): Optimize allocation size to power of 2
template <typename T, int MAX_SIZE> class StaticList {
private:
  struct Node {
    T value;
    Node *next_ptr{nullptr};
    Node *prev_ptr{nullptr};
  };

public:
  class Iterator {
  public:
    Node *ptr_;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

  public:
    Iterator(Node *ptr) : ptr_{ptr} {}

    T &operator*() { return ptr_->value; }

    Iterator &operator++() {
      ptr_ = ptr_->next_ptr;
      return *this;
    }

    Iterator &operator--() {
      ptr_ = ptr_->prev_ptr;
      return *this;
    }

    bool operator==(const Iterator &rhs) const { return ptr_ == rhs.ptr_; }

    bool operator!=(const Iterator &rhs) const { return ptr_ != rhs.ptr_; }

    explicit operator bool() const { return ptr_ != nullptr; }

    Node &Data() { return *ptr_; }
  };

public:
  StaticList() {}

  Iterator Begin() const { return {head_}; }

  Iterator End() const { return {tail_}; }

  size_t Size() const { return size_; }

  typename std::enable_if<std::is_copy_constructible<T>::value, bool>::type
  PushBack(const T &value) {
    return PushBackImpl(value);
  }

  typename std::enable_if<std::is_move_constructible<T>::value, bool>::type
  PushBack(T &&value) {
    return PushBackImpl(value);
  }

  typename std::enable_if<std::is_copy_constructible<T>::value, bool>::type
  PushFront(const T &value) {
    return PushFrontImpl(value);
  }

  typename std::enable_if<std::is_move_constructible<T>::value, bool>::type
  PushFront(T &&value) {
    return PushFrontImpl(value);
  }

  typename std::enable_if<std::is_copy_constructible<T>::value, bool>::type
  Insert(Iterator it, const T &value) {
    return InsertImpl(value);
  }

  typename std::enable_if<std::is_move_constructible<T>::value, bool>::type
  Insert(Iterator it, T &&value) {
    return InsertImpl(value);
  }

  bool Erase(Iterator it) {
    if (!it)
      return true;

    if (std::prev(it)) {
      std::prev(it).Data().next_ptr = it.Data().next_ptr;
    }
    if (std::next(it)) {
      std::next(it).Data().prev_ptr = it.Data().prev_ptr;
    }

    if (it == Begin()) {
      head_ = it.Data().next_ptr;
    }
    if (it == End()) {
      tail_ = it.Data().prev_ptr;
    }

    size_ -= 1;

    return allocator_.Delete(&it.Data());
  }

private:
  bool PushBackImpl(auto &&value) {
    auto data = allocator_.New();
    if (data == nullptr) [[unlikely]]
      return false;

    new (data) Node{.value{std::forward<decltype(value)>(value)},
                    .next_ptr{nullptr},
                    .prev_ptr{nullptr}};

    if (head_) [[likely]] {
      tail_->next_ptr = data;
      data->prev_ptr = tail_;
      tail_ = data;
    } else {
      head_ = tail_ = data;
    }

    size_ += 1;

    return true;
  }

  bool PushFrontImpl(auto &&value) {
    auto data = allocator_.New();
    if (data == nullptr) [[unlikely]]
      return false;

    new (data) Node{.value{std::forward<decltype(value)>(value)},
                    .next_ptr{nullptr},
                    .prev_ptr{nullptr}};

    if (head_) [[likely]] {
      data->next_ptr = head_;
      head_->prev_ptr = data;
      head_ = data;
    } else {
      head_ = tail_ = data;
    }

    size_ += 1;

    return true;
  }

  bool InsertImpl(Iterator it, auto &&value) {
    if (!it && head_ != nullptr) [[unlikely]]
      return false;

    // TODO(@vspm): Handle case insert object using iterator from other list

    auto data = allocator_.New();
    if (!data) [[unlikely]]
      return false;

    new (data) Node{
        .value{std::forward<decltype(value)>(value)},
        .next_ptr{nullptr},
        .prev_ptr{nullptr},
    };

    if (head_ != nullptr) [[likely]] {
      data->next_ptr = it.Data().next_ptr;
      it.Data().next_ptr = data;
      data->prev_ptr = &it.Data();

      if (it == End())
        tail_ = data;
    }
    if (head_ == nullptr) {
      head_ = tail_ = data;
    }

    size_ += 1;

    return true;
  }

private:
  size_t size_{0};
  Node *head_{nullptr};
  Node *tail_{nullptr};

  StaticListAllocator<Node, MAX_SIZE> allocator_{};
};

} // namespace hermes::container
