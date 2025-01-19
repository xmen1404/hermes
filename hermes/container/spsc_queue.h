#pragma once

#include <atomic>
#include <memory>
#include <new>
#include <optional>
#include <string>

namespace hermes::container {

/**
 * Single Producer Single Consumer Queue
 */
template <typename T, uint32_t MAX_SIZE> class SpscQueue {
public:
  SpscQueue(const SpscQueue &_) = delete;
  SpscQueue &operator=(const SpscQueue &_) = delete;

  SpscQueue() {
    // allocate one extra slot for dummy node
    data_ = static_cast<T *>(operator new[](sizeof(T) * (MAX_SIZE + 1)));
    write_index_ = 0;
    read_index_ = 0;
  }

  ~SpscQueue() {
    while (!IsEmpty())
      PopFront();
    operator delete[](data_);
  }

public:
  // Emplace value at the begin of the queue
  // Only producer thread should call this method
  template <typename... Args> bool Write(Args &&...args) {
    const auto curr_read = read_index_.load(std::memory_order_acquire);
    const auto curr_write = write_index_.load(std::memory_order_relaxed);
    const auto next_write = (curr_write + 1) % (MAX_SIZE + 1);

    if (next_write != curr_read) {
      new (data_ + curr_write) T{std::forward<decltype(args)>(args)...};
      write_index_.store(next_write, std::memory_order_release);
      return true;
    }

    // queue is full
    return false;
  }

  // Read the last value from the queue
  // Only consumer thread should can this method
  bool Read(T &record) {
    auto curr_write = write_index_.load(std::memory_order_acquire);
    auto curr_read = read_index_.load(std::memory_order_relaxed);

    if (curr_read != curr_write) {
      if constexpr (std::is_move_assignable<T>::value)
        record = std::move(data_[curr_read]);
      else
        record = data_[curr_read];

      // destroy old object
      data_[curr_read].~T();

      read_index_.store((curr_read + 1) % (MAX_SIZE + 1),
                        std::memory_order_release);
      return true;
    }

    // queue is empty
    return false;
  }

  std::optional<T> Read() {
    auto curr_write = write_index_.load(std::memory_order_acquire);
    auto curr_read = read_index_.load(std::memory_order_relaxed);

    if (curr_read != curr_write) {
      auto ret = std::optional<T>{};
      if constexpr (std::is_move_constructible<T>::value)
        ret = std::move(data_[curr_read]);
      else
        ret = data_[curr_read];

      // destroy old object
      data_[curr_read].~T();
      read_index_.store((curr_read + 1) % (MAX_SIZE + 1),
                        std::memory_order_release);

      return ret;
    }

    // queue is empty
    return {};
  }

  // queue must not be empty
  void PopFront() noexcept {
    auto curr_read = read_index_.load(std::memory_order_relaxed);
    data_[curr_read].~T();
    read_index_.store((curr_read + 1) % (MAX_SIZE + 1),
                      std::memory_order_release);
  }

  template <typename Functor> bool ConsumeOne(const Functor &functor) {
    return true;
  }

  template <typename Functor> bool ConsumeAll(const Functor &functor) {
    return true;
  }

  bool ReadAvailable() const noexcept { return !IsEmpty(); }

  bool WriteAvailable() const noexcept {
    const auto curr_read = read_index_.load(std::memory_order_acquire);
    const auto curr_write = write_index_.load(std::memory_order_acquire);
    return (curr_write + 1) % (MAX_SIZE + 1) != curr_read;
  }

  bool IsEmpty() const noexcept {
    const auto curr_read = read_index_.load(std::memory_order_acquire);
    const auto curr_write = write_index_.load(std::memory_order_acquire);
    return curr_read == curr_write;
  }

  size_t SizeGuess() const noexcept {
    const auto curr_read = read_index_.load(std::memory_order_acquire);
    const auto curr_write = write_index_.load(std::memory_order_acquire);

    auto ret = curr_write - curr_read;
    return ret < 0 ? (ret + MAX_SIZE + 1) : ret;
  }

  static constexpr uint32_t Capacity() noexcept { return MAX_SIZE; }

private:
  using AtomicIndex = std::atomic<uint32_t>;
  static constexpr std::size_t CACHE_LINE_SIZE =
#ifdef __cpp_lib_hardware_interference_size
      std::hardware_destructive_interference_size;
#else
      64;
#endif

private:
  T *data_;

  alignas(CACHE_LINE_SIZE) AtomicIndex write_index_;
  alignas(CACHE_LINE_SIZE) AtomicIndex read_index_;

  char pad_0_[CACHE_LINE_SIZE - sizeof(AtomicIndex)];
};

} // namespace hermes::container
