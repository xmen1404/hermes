#pragma once

#include <atomic>
#include <memory>
#include <type_traits>

namespace hermes::container {

/**
 * @brief A simplified unique pointer that exclusively owns a dynamically
 * allocated object.
 *
 * @tparam T The type of the managed object.
 */
template <typename T> class UniquePtr {
public:
  /**
   * @brief Constructs a UniquePtr that takes ownership of the given pointer.
   *
   * @param ptr A pointer to a dynamically allocated object of type T (defaults
   * to nullptr).
   */
  explicit UniquePtr(T *ptr = nullptr) { data_ = ptr; }

  /**
   * @brief Move constructor that transfers ownership from another UniquePtr.
   *
   * @param rhs An rvalue reference to another UniquePtr.
   */
  UniquePtr(UniquePtr &&rhs) : data_{rhs.data_} { rhs.data_ = nullptr; }

  /**
   * @brief Destructor that deletes the managed object.
   */
  ~UniquePtr() noexcept { delete data_; }

  // Deleted copy assignment operators to enforce unique ownership.
  UniquePtr &operator=(UniquePtr &rhs) = delete;
  UniquePtr &operator=(const UniquePtr &rhs) = delete;

  /**
   * @brief Move assignment operator that transfers ownership.
   *
   * @param rhs An rvalue reference to another UniquePtr.
   * @return A reference to this UniquePtr.
   */
  UniquePtr &operator=(UniquePtr &&rhs) {
    std::swap(data_, rhs.data_);
    return *this;
  }

public:
  /**
   * @brief Returns the raw pointer to the managed object.
   *
   * @return A pointer to the managed object.
   */
  T *Get() const noexcept { return data_; }

  /**
   * @brief Explicit conversion to bool to check if the UniquePtr owns an
   * object.
   *
   * @return true if the managed pointer is not nullptr, false otherwise.
   */
  explicit operator bool() const noexcept { return data_ != nullptr; }

private:
  T *data_;
};

/**
 * @brief Creates a UniquePtr managing a new instance of T.
 *
 * Constructs an object of type T using the provided arguments and returns a
 * UniquePtr that owns it.
 *
 * @tparam T The type of the object to be created.
 * @tparam Args The types of the arguments used to construct the object.
 * @param params Parameters forwarded to T's constructor.
 * @return A UniquePtr that manages the newly created object.
 */
template <typename T, typename... Args>
inline UniquePtr<T> MakeUnique(Args &&...params) {
  return UniquePtr<T>{new T{std::forward<decltype(params)>(params)...}};
}

/**
 * @brief Control block for SharedPtr, managing reference counts.
 */
struct SpControlBlock {
  std::atomic<size_t> ref_cnt;      ///< Shared pointer count.
  std::atomic<size_t> weak_ref_cnt; ///< Weak pointer count.

  SpControlBlock() = delete;
  SpControlBlock(bool is_weak) : ref_cnt(!is_weak), weak_ref_cnt(is_weak) {}
};

template <typename T> class WeakPtr;

/**
 * @brief A simple shared pointer implementation.
 * @tparam T The type of the managed object.
 */
template <typename T> class SharedPtr {
public:
  /// Default constructor: creates an empty SharedPtr.
  SharedPtr() {}

  /// Constructs a SharedPtr that owns the given pointer.
  SharedPtr(T *ptr) : data_ptr_(ptr), ctrl_ptr_(new SpControlBlock(false)) {}

  SharedPtr(T *data_ptr, SpControlBlock *ctrl_ptr)
      : data_ptr_(data_ptr), ctrl_ptr_(ctrl_ptr) {}

  /// Copy constructor: increments the reference count.
  SharedPtr(const SharedPtr &rhs) {
    if (rhs.ctrl_ptr_ != nullptr) {
      data_ptr_ = rhs.data_ptr_;
      ctrl_ptr_ = rhs.ctrl_ptr_;
      ctrl_ptr_->ref_cnt.fetch_add(1, std::memory_order_relaxed);
    }
  }

  /// Move constructor: increments the reference count.
  SharedPtr(SharedPtr &&rhs) {
    if (rhs.ctrl_ptr_ != nullptr) {
      data_ptr_ = rhs.data_ptr_;
      ctrl_ptr_ = rhs.ctrl_ptr_;
      ctrl_ptr_->ref_cnt.fetch_add(1, std::memory_order_relaxed);
    }
  }

  /// Destructor: releases ownership.
  ~SharedPtr() { Reset(); }

public:
  /// Releases ownership. Deletes the managed object if this was the last owner.
  void Reset() {
    if (!ctrl_ptr_)
      return;
    auto prev_cnt = ctrl_ptr_->ref_cnt.fetch_add(-1, std::memory_order_acq_rel);
    if (prev_cnt == 1) {
      delete data_ptr_;
      if (!ctrl_ptr_->weak_ref_cnt.load(std::memory_order_acquire))
        delete ctrl_ptr_;
    }
    data_ptr_ = nullptr;
    ctrl_ptr_ = nullptr;
  }

  /// Resets to manage a new pointer.
  void Reset(T *ptr) {
    Reset();
    ctrl_ptr_ = new SpControlBlock(false);
    data_ptr_ = ptr;
  }

  /// Returns the managed pointer.
  T *Get() noexcept { return data_ptr_; }

  /// Returns true if this is the unique owner.
  bool IsUnique() const noexcept {
    return !ctrl_ptr_ ||
           ctrl_ptr_->ref_cnt.load(std::memory_order_relaxed) == 1;
  }

private:
  SpControlBlock *ctrl_ptr_{nullptr}; ///< Pointer to the control block.
  T *data_ptr_{nullptr};              ///< Pointer to the managed object.

private:
  friend class WeakPtr<T>;
};

template <typename T, typename... Args>
inline static SharedPtr<T> MakeShared(Args &&...params) {
  return SharedPtr<T>(new T{std::forward<Args>(params)...},
                      new SpControlBlock{false});
}

/**
 * @brief A non-owning weak pointer.
 *
 * @tparam T Type of the object.
 */
template <typename T> class WeakPtr {
public:
  /// Default constructor.
  constexpr WeakPtr() noexcept {}

  /// Copy constructor: increments weak count if control block exists.
  WeakPtr(const WeakPtr &rhs) noexcept {
    if (rhs.ctrl_ptr_ != nullptr) {
      ctrl_ptr_ = rhs.ctrl_ptr_;
      data_ptr_ = rhs.data_ptr_;
      ctrl_ptr_->weak_ref_cnt.fetch_add(1, std::memory_order_relaxed);
    }
  }

  /// Construct from SharedPtr: increments weak count if control block exists.
  WeakPtr(const SharedPtr<T> &rhs) noexcept {
    if (rhs.ctrl_ptr_ != nullptr) {
      ctrl_ptr_ = rhs.ctrl_ptr_;
      data_ptr_ = rhs.data_ptr_;
      ctrl_ptr_->weak_ref_cnt.fetch_add(1, std::memory_order_relaxed);
    }
  }

  /**
   * @brief Assign from a SharedPtr.
   *
   * Releases current control block and links to the new one.
   */
  WeakPtr &operator=(const SharedPtr<T> &rhs) noexcept {
    Reset();
    if (rhs.ctrl_ptr_ != nullptr) {
      ctrl_ptr_ = rhs.ctrl_ptr_;
      data_ptr_ = rhs.data_ptr_;
      ctrl_ptr_->weak_ref_cnt.fetch_add(1, std::memory_order_relaxed);
    }
    return *this;
  }

  /// @brief Checks if the managed object has expired.
  bool IsExpired() const noexcept {
    return ctrl_ptr_ == nullptr ||
           !ctrl_ptr_->ref_cnt.load(std::memory_order_relaxed);
  }

  /**
   * @brief Locks the weak pointer to return a SharedPtr.
   *
   * Increments the strong count if object exists; else returns empty SharedPtr.
   */
  SharedPtr<T> Lock() {
    if (ctrl_ptr_ == nullptr)
      return {};

    auto prev_cnt = ctrl_ptr_->ref_cnt.fetch_add(1, std::memory_order_acquire);
    if (prev_cnt == 0) {
      ctrl_ptr_->ref_cnt.fetch_add(-1, std::memory_order_acq_rel);
      return {};
    }
    return SharedPtr<T>{data_ptr_, ctrl_ptr_};
  }

  /**
   * @brief Releases the weak pointer.
   *
   * Decrements weak count and deletes the control block if needed.
   */
  void Reset() {
    if (ctrl_ptr_ == nullptr)
      return;
    auto prev_cnt =
        ctrl_ptr_->weak_ref_cnt.fetch_add(-1, std::memory_order_acq_rel);
    if (prev_cnt == 1 && !ctrl_ptr_->ref_cnt.load(std::memory_order_acquire))
      delete ctrl_ptr_;
    ctrl_ptr_ = nullptr;
    data_ptr_ = nullptr;
  }

private:
  SpControlBlock *ctrl_ptr_{nullptr}; ///< Pointer to the control block.
  T *data_ptr_{nullptr};              ///< Pointer to the managed object.
};

} // namespace hermes::container
