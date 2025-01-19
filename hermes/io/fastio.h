#pragma once

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <type_traits>
#include <unistd.h>

#include <glog/logging.h>

namespace hermes::io {

template <typename T, int MAX_BUFFER_SIZE = 1 << 13,
          typename = std::enable_if_t<std::is_integral<T>::value>>
class IntegralFastIO {
public:
  void Init(const char *path) { Init(open(path, O_RDONLY)); }

  void Init(const int fd) noexcept {
    fd_ = fd;
    file_offset_ = 0;

    struct stat stat_buf;
    CHECK(fstat(fd, &stat_buf) == 0)
        << "Failed to init IntegralFastIO, could not get file size";
    fstat(fd, &stat_buf);
    file_size_ = stat_buf.st_size;

    buffer_base_ = new char[MAX_BUFFER_SIZE + sysconf(_SC_PAGESIZE)];

    ReadToBuffer();

    is_good_ = end_ != begin_;

    LOG_FIRST_N(INFO, 100) << "Initilized FastIO reader with file size = "
                           << file_size_;
  }

  inline bool IsGood() const noexcept { return is_good_; }

  T ReadOne() {
    T ret = 0;

    char c = NextChar();
    while (IsDigit(c)) {
      ret = ret * 10 + (c - '0');
      c = NextChar();
    }

    return ret;
  }

public:
  static inline bool IsDigit(char c) noexcept {
    return static_cast<unsigned char>(c - '0') < 10;
  }

private:
  inline char NextChar() {
    if (begin_ >= end_) [[unlikely]]
      ReadToBuffer();
    return buffer_[begin_++];
  }

  void ReadToBuffer() {
    munmap(buffer_, end_);
    file_offset_ += end_;

    begin_ = end_ = 0;
    buffer_ = buffer_base_;
    *buffer_ = '\0';

    const auto read_size =
        std::min(file_size_ - file_offset_, size_t(MAX_BUFFER_SIZE));
    const auto addr = mmap(buffer_base_, read_size, PROT_READ, MAP_PRIVATE, fd_,
                           file_offset_);

    if (addr != (void *)-1) [[likely]] {
      buffer_ = reinterpret_cast<char *>(addr);
      end_ = read_size;
    }

    is_good_ = begin_ != end_;
  }

private:
  bool is_good_{false};
  int begin_{0}, end_{0};

  char *buffer_base_{0};
  char *buffer_{0};

  size_t file_size_{0};
  size_t file_offset_{0};
  int fd_{-1};
};

} // namespace hermes::io
