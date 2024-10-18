#pragma once

#include <atomic>
#include <fcntl.h>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <glog/logging.h>

namespace hermes::container {

/**
 * Single-Producer-Single-Consumer Queue mmap file backed
 */
template <typename T> class SpscQueue {
public:
  SpscQueue() {}

  ~SpscQueue() {
    LOG(INFO) << "Closing mmap file data";

    munmap(mmap_ptr_, file_size_);
    close(fd_);
  }

public:
  void Init(const std::string &file_path, const size_t file_size,
            const bool reset) noexcept {
    LOG(INFO) << "Initializing spsc_queue at path: " << file_path;

    CHECK(file_size > HEADER_SIZE)
        << "File size (" << file_size
        << " bytes) should greater than header size (" << HEADER_SIZE
        << " bytes)";

    file_size_ = file_size;
    fd_ = open(file_path.c_str(), O_RDWR | O_CREAT, 0666);

    CHECK(fd_ != -1) << "Error opening file";
    if (reset)
      CHECK(ftruncate(fd_, file_size) != -1) << "Error resizing file";

    mmap_ptr_ = static_cast<char *>(
        mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0));

    CHECK(data_ != MAP_FAILED) << "Error mapping file at path: " << file_path;

    head_idx_ = reinterpret_cast<std::atomic<int> *>(mmap_ptr_);
    tail_idx_ = reinterpret_cast<std::atomic<int> *>(mmap_ptr_ + CACHE_LINE);

    data_ = static_cast<T *>(mmap_ptr_ + HEADER_SIZE);
    size_ = 0;
    max_size_ = (file_size - 2 * sizeof(std::atomic<int>)) / sizeof(T);

    head_idx_->store(0, std::memory_order_release);
    tail_idx_->store(1, std::memory_order_release);

    LOG(INFO) << "Successfully initialized spsc_queue at path: " << file_path;
  }

  typename std::enable_if<std::is_copy_constructible<T>::value, bool>::type
  Push(const T &data) {
    return PushImpl(data);
  }

  typename std::enable_if<std::is_move_constructible<T>::value, void>::type
  Push(T &&data) {
    return PushImpl(data);
  }

  std::optional<T> Pop() {
    // TODO: implement
  }

private:
  bool PushImpl(auto &&value) {
    auto head = head_idx_->load(std::memory_order_acquire);
    auto tail = tail_idx_->load(std::memory_order_acquire);
    if (head == tail) [[unlikely]]
      return false;

    auto *obj_ptr = static_cast<T *>(data_ + tail);
    new (obj_ptr) T{std::forward<decltype(value)>(value)};

    auto new_tail = (tail + 1) % max_size_;
    tail_idx_->store(new_tail, std::memory_order_release);

    return true;
  }

private:
  static constexpr size_t CACHE_LINE = 64;
  static constexpr size_t HEADER_SIZE = 2 * CACHE_LINE;

private:
  int fd_;
  size_t file_size_;
  char *mmap_ptr_;

  T *data_;
  int max_size_;
  int size_;
  std::atomic<int> *head_idx_;
  std::atomic<int> *tail_idx_;
};

} // namespace hermes::container
