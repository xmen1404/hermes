#pragma once

#include <glog/logging.h>

#include <cstring>
#include <iterator>
#include <type_traits>
#include <utility>

namespace hermes::container {

namespace detail {

template <typename T, int MAX_SIZE>
class StaticListAllocator {
 public:
  StaticListAllocator() {
    data_ = static_cast<T *>(operator new[](sizeof(T) * MAX_SIZE));
    size_ = 0;

    free_cnt_ = MAX_SIZE;
    free_list_ = new T *[MAX_SIZE];
    for (auto i = 0; i < free_cnt_; ++i) free_list_[i] = data_ + i;
  }

  ~StaticListAllocator() noexcept {
    operator delete[](data_);
    delete[] free_list_;
  }

 public:
  T *New() noexcept {
    if (!free_cnt_) [[unlikely]]
      return nullptr;

    free_cnt_ -= 1;
    return free_list_[free_cnt_];
  }

  /**
   * @note: assume ptr align correctly with the buffer address (data_)
   */
  bool Delete(T *ptr) noexcept {
    free_list_[free_cnt_] = ptr;
    free_cnt_ += 1;

    return true;
  }

 private:
  size_t size_;
  T *data_;

  T **free_list_;
  int free_cnt_;
};

struct ListNodeBase {
  ListNodeBase *prev;
  ListNodeBase *next;

  void Hook(ListNodeBase *pos) noexcept {
    if (pos == this) [[unlikely]]
      return;

    prev = pos->prev;
    pos->prev->next = this;

    next = pos;
    pos->prev = this;
  }

  void UnHook() noexcept {
    prev->next = next;
    next->prev = prev;
  }
};

struct ListNodeHeader : ListNodeBase {};

template <typename T>
struct ListNode : ListNodeBase {
  T value;
};

template <typename T>
class ListIterator {
 public:
  ListNodeBase *ptr_;

 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T *;
  using reference = T &;

 public:
  ListIterator(ListNodeBase *ptr) : ptr_{ptr} {}

  reference operator*() { return static_cast<ListNode<T> *>(ptr_)->value; }

  // prefix increment
  ListIterator &operator++() noexcept {
    ptr_ = ptr_->next;
    return *this;
  }

  // postfix increment
  ListIterator operator++(int) noexcept {
    ListIterator temp = *this;
    ptr_ = ptr_->next;
    return temp;
  }

  // prefix decrement
  ListIterator &operator--() noexcept {
    ptr_ = ptr_->prev;
    return *this;
  }

  // postfix decrement
  ListIterator operator--(int) noexcept {
    auto temp = *this;
    ptr_ = ptr_->next;
    return temp;
  }

  bool operator==(const ListIterator &rhs) const noexcept {
    return ptr_ == rhs.ptr_;
  }

  bool operator!=(const ListIterator &rhs) const noexcept {
    return ptr_ != rhs.ptr_;
  }

  explicit operator bool() const noexcept { return ptr_ != nullptr; }
};

}  // namespace detail

template <typename T, int MAX_SIZE>
class StaticList {
 public:
  typedef detail::ListIterator<T> iterator;

 public:
  StaticList() {
    head_ = static_cast<detail::ListNodeBase *>(allocator_.New());

    CHECK(head_ != nullptr)
        << "Failed to init StaticList because of failed memory allocation";

    new (head_) detail::ListNodeBase{};
    head_->prev = head_->next = head_;
    size_ = 0;
  }

  StaticList(const StaticList &rhs) {
    // implement me
  }

  StaticList(StaticList &&rhs) {
    // implement me
  }

  iterator Begin() const noexcept { return {head_->next}; }

  iterator End() const noexcept { return {head_}; }

  size_t Size() const noexcept { return size_; }

  bool IsEmpty() const noexcept { return !size_; }

  typename std::enable_if<std::is_copy_constructible<T>::value, bool>::type
  PushBack(const T &value) {
    return PushBackImpl(std::forward<const T>(value));
  }

  typename std::enable_if<std::is_move_constructible<T>::value, bool>::type
  PushBack(T &&value) {
    return PushBackImpl(std::forward<T>(value));
  }

  typename std::enable_if<std::is_copy_constructible<T>::value, bool>::type
  PushFront(const T &value) {
    return PushFrontImpl(std::forward<const T>(value));
  }

  typename std::enable_if<std::is_move_constructible<T>::value, bool>::type
  PushFront(T &&value) {
    return PushFrontImpl(std::forward<T>(value));
  }

  typename std::enable_if<std::is_copy_constructible<T>::value, bool>::type
  Insert(iterator it, const T &value) {
    return InsertImpl(it, std::forward<const T>(value));
  }

  typename std::enable_if<std::is_move_constructible<T>::value, bool>::type
  Insert(iterator it, T &&value) {
    return InsertImpl(it, std::forward<T>(value));
  }

  bool Erase(iterator it) {
    if (it == End()) return true;

    it.ptr_->UnHook();
    size_ -= 1;

    return allocator_.Delete(static_cast<detail::ListNode<T> *>(it.ptr_));
  }

 private:
  bool PushBackImpl(auto &&value) {
    auto data = allocator_.New();
    if (data == nullptr) [[unlikely]]
      return false;

    new (data) detail::ListNode<T>{nullptr, nullptr,
                                   std::forward<decltype(value)>(value)};

    data->Hook(head_);
    size_ += 1;

    return true;
  }

  bool PushFrontImpl(auto &&value) {
    auto data = allocator_.New();
    if (data == nullptr) [[unlikely]]
      return false;

    new (data) detail::ListNode<T>{nullptr, nullptr,
                                   std::forward<decltype(value)>(value)};

    data->Hook(head_->next);

    size_ += 1;

    return true;
  }

  bool InsertImpl(iterator it, auto &&value) {
    // TODO(@vspm): Handle case insert object using iterator from other list

    auto data = allocator_.New();
    if (data == nullptr) [[unlikely]]
      return false;

    new (data) detail::ListNode<T>{nullptr, nullptr,
                                   std::forward<decltype(value)>(value)};

    data->Hook(it.ptr_);
    size_ += 1;

    return true;
  }

 private:
  size_t size_{0};
  detail::ListNodeBase *head_{nullptr};

  // need one mem block for dummy header node
  detail::StaticListAllocator<detail::ListNode<T>, MAX_SIZE + 1> allocator_{};
};

}  // namespace hermes::container
