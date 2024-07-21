#include <cstdio>
#include <fcntl.h>
#include <type_traits>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <glog/logging.h>

namespace hermes::fastio {

template <typename T, int MAX_BUFFER_SIZE = 1 << 13,
          typename = std::enable_if_t<std::is_integral<T>::value>>
class IntegralFastIO {
public:
  void Init(const char* path) {
    Init(open(path, O_RDONLY));
  }

  void Init(const int fd) {
    fd_ = fd;
    file_offset_ = 0;

    struct stat stat_buf;
    CHECK(fstat(fd, &stat_buf) == 0) << "Failed to init IntegralFastIO, could not get file size";  
    file_size_ = stat_buf.st_size;

    buffer_base_ = new char[MAX_BUFFER_SIZE + sysconf (_SC_PAGESIZE)];
    ReadToBuffer();
  }

  bool ReadOne(T &ret) {
    // skip whitespace
    while (begin_ != end_) {
      if (buffer_[begin_] >= '0' && buffer_[begin_] <= '9')
        break;
      begin_ += 1;
      if (begin_ == end_)
        ReadToBuffer();
    }

    if (begin_ == end_) [[unlikely]]
      return false;

    ret = 0;
    while (begin_ != end_) {
      if (buffer_[begin_] < '0' || buffer_[begin_] > '9')
        break;
      ret = ret * 10 + (buffer_[begin_++] - '0');
      if (begin_ == end_)
        ReadToBuffer();
    }

    return true;
  }

private: 
  void ReadToBuffer() {
    munmap(buffer_, end_);
    
    file_offset_ += end_;

    const auto read_size = std::min(file_size_ - file_offset_, size_t(MAX_BUFFER_SIZE)); 
    const auto addr = mmap(buffer_base_, read_size, PROT_READ, MAP_PRIVATE, fd_, file_offset_);
    
    begin_ = end_ = 0;
    if (addr != (void*)-1) [[likely]] {
      buffer_ = reinterpret_cast<char*>(addr);
      end_ = read_size;
    }
  }

private:
  int begin_{0}, end_{0};

  void* buffer_base_{0};
  char* buffer_{0};

  size_t file_size_{0};
  size_t file_offset_{0};
  int fd_{-1};
};

} // namespace hermes::fastio

