#pragma once

#include <cassert>
#include <glog/logging.h>
#include <list>
#include <type_traits>
#include <vector>

namespace hermes::container {

namespace detail {

struct HashMapNodeBase {
  HashMapNodeBase *prev;
  HashMapNodeBase *next;

  void Hook(HashMapNodeBase *pos) noexcept {
    if (pos == this) [[unlikely]]
      return;

    prev = pos->prev;
    pos->prev->next = this;

    next = pos;
    pos->prev = this;
  }

  // @NOTE: user must handle data deallocation
  void UnHook() noexcept {
    prev->next = next;
    next->prev = prev;
  }
};

struct HashMapNodeHeader : HashMapNodeBase {};

template <class Key, class T> struct HashMapNode : HashMapNodeBase {
  std::pair<Key, T> p_data;
  size_t bucket_id;

  HashMapNode(std::pair<Key, T> &&p_data, size_t bucket_id)
      : p_data(std::move(p_data)), bucket_id(bucket_id) {}
};

template <class Key, class T> class HashMapIterator {
public:
  HashMapNodeBase *ptr_;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = std::pair<Key, T>;
  using difference_type = std::ptrdiff_t;
  using pointer = std::pair<Key, T> *;
  using reference = std::pair<Key, T> &;

public:
  explicit HashMapIterator(HashMapNodeBase *ptr) : ptr_(ptr) {}

  // @ASSUME: current iterator is HashMapNode, not HashMapNodeHeader
  inline size_t BucketId() const noexcept {
    return static_cast<HashMapNode<Key, T> *>(ptr_)->bucket_id;
  }

  // reference iterator
  // @ASSUME: current iterator is HashMapNode, not HashMapNodeHeader
  reference operator*() {
    return static_cast<HashMapNode<Key, T> *>(ptr_)->p_data;
  }

  pointer operator->() noexcept {
    return &static_cast<HashMapNode<Key, T> *>(ptr_)->p_data;
  }

  // prefix increment
  // @NOTE: if current iterator is end of the list, return iterator will be
  // circled around (first element of the list)
  HashMapIterator &operator++() noexcept {
    ptr_ = ptr_->next;
    return *this;
  }

  // postfix increment
  HashMapIterator operator++(int) noexcept {
    auto ret = *this;
    ptr_ = ptr_->next;
    return ret;
  }

  // prefix decrement
  HashMapIterator &operator--() noexcept {
    ptr_ = ptr_->prev;
    return *this;
  }

  // postfix decrement
  HashMapIterator operator--(int) noexcept {
    auto ret = *this;
    ptr_ = ptr_->prev;
    return ret;
  }

  bool operator==(const HashMapIterator &rhs) const noexcept {
    return ptr_ == rhs.ptr_;
  }

  bool operator!=(const HashMapIterator &rhs) const noexcept {
    return ptr_ != rhs.ptr_;
  }
};

} // namespace detail

/**
 * Implementation of std::unordered_map
 */
template <class Key, class T, class Hash = std::hash<Key>,
          double LOAD_FACTOR = 1.00>
class HashMap {
public:
  typedef detail::HashMapIterator<Key, T> iterator;
  typedef detail::HashMapNode<Key, T> Node;
  typedef detail::HashMapNodeBase NodeBase;
  typedef std::pair<Key, T> PType;

public:
  HashMap() {}

  ~HashMap() {
    auto it = Begin();
    while (it.ptr_ != head_) {
      auto *node_base_ptr = it.ptr_;
      it++;

      delete static_cast<Node *>(node_base_ptr);
    }
  }

  // TODO(@vspm): implement other ctor overload

  void Init() {
    head_->next = head_->prev = head_;

    size_ = 0;
    bucket_cnt_ = 1;

    buckets_.push_back(nullptr);
  }

public:
  inline bool IsEmpty() const noexcept { return !size_; }

  inline size_t Size() const noexcept { return size_; }

  inline size_t BucketSize() const noexcept { return bucket_cnt_; }

  inline bool Contains(const Key &key) const noexcept {
    const auto it = Find(key);
    return it != End();
  }

  iterator Find(const Key &key) const noexcept {
    const auto bucket_id = KeyToBucketId(key);
    if (buckets_[bucket_id] == nullptr)
      return iterator{head_};

    auto *ptr = buckets_[bucket_id];
    while (ptr != head_) {
      const auto *node_ptr = static_cast<Node *>(ptr);
      if (node_ptr->bucket_id != bucket_id)
        break;
      if (node_ptr->p_data.first == key)
        return iterator{ptr};
      ptr = ptr->next;
    }
    return iterator{head_};
  }

  // @RETURN:
  // - iterator points to inserted (key, value)
  // - bool represent whether the insertion happened
  std::pair<iterator, bool> Insert(PType &&p_data) {
    auto it = Find(p_data.first);
    if (it.ptr_ != head_) {
      // key already existed in the hash_table
      it->second = std::move(p_data.second);
      return {it, false};
    }
    return {InsertNew(std::move(p_data)), true};
  }

  iterator Erase(iterator it) {
    if (it == End())
      return iterator{head_};

    const auto bucket_id = KeyToBucketId(it->first);
    auto *next_ptr = it.ptr_->next;
    it.ptr_->UnHook();

    if (it.ptr_ == buckets_[bucket_id])
      buckets_[bucket_id] =
          (next_ptr != head_ && iterator{next_ptr}.BucketId() == bucket_id)
              ? next_ptr
              : nullptr;

    delete static_cast<Node *>(it.ptr_);

    return iterator{next_ptr};
  }

  // @RETURN: iterator points to first pair (key, value); End() iterator if
  // hash_map is empty
  iterator Begin() noexcept {
    for (size_t id = 0; id < bucket_cnt_; ++id) {
      if (buckets_[id] != nullptr) {
        return iterator{buckets_[id]};
      }
    }

    return iterator{head_};
  }

  inline iterator End() noexcept { return iterator{head_}; }

  T &operator[](const Key &key) {
    static_assert(std::is_default_constructible<T>::value,
                  "operator[] requires value type to be default-constructible");

    auto it = Find(key);
    if (it.ptr_ == head_) {
      it = InsertNew(PType{key, T{}});
    }
    return it->second;
  }

private:
  inline iterator InsertNew(PType &&p_data) {
    auto *node = new Node{std::move(p_data), 0};
    InsertNew(node);
    if (double(size_) >= LOAD_FACTOR * bucket_cnt_) [[unlikely]]
      Rehash();
    return iterator{node};
  }

  void InsertNew(NodeBase *node_base_ptr) noexcept {
    auto *node_ptr = static_cast<Node *>(node_base_ptr);
    const auto bucket_id = KeyToBucketId(node_ptr->p_data.first);
    node_ptr->bucket_id = bucket_id;
    auto *ptr = buckets_[bucket_id];
    if (ptr == nullptr) {
      for (auto id = bucket_id + 1; id < bucket_cnt_; ++id) {
        if (buckets_[id] != nullptr) {
          ptr = buckets_[id];
          break;
        }
      }
      if (ptr == nullptr)
        ptr = head_;
    }

    node_base_ptr->Hook(ptr);
    buckets_[bucket_id] = node_base_ptr;
    size_ += 1;
  }

  void Rehash() {
    auto it = Begin();

    bucket_cnt_ *= 2;
    size_ = 0;
    head_->prev = head_->next = head_;

    auto old_buckets = std::move(buckets_);
    buckets_ = std::vector<NodeBase *>(bucket_cnt_, nullptr);

    while (it.ptr_ != head_) {
      auto *node_base_ptr = it.ptr_;
      it++;
      InsertNew(node_base_ptr);
    }
  }

  size_t KeyToBucketId(const Key &key) const noexcept {
    return Hash()(key) % bucket_cnt_;
  }

private:
  size_t size_;
  size_t bucket_cnt_;

  NodeBase dummy_{};
  NodeBase *head_{&dummy_};
  std::vector<NodeBase *> buckets_;
};

} // namespace hermes::container
