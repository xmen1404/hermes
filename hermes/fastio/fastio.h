#include <cstdio>
#include <type_traits>
#include <fcntl.h>
#include <unistd.h>

namespace hermes::fastio {

template <typename T, int MAX_BUFFER_SIZE = 1<<13, typename = std::enable_if_t<std::is_integral<T>::value>>
class IntegralFastIO {
public:
  IntegralFastIO() = delete;
  
  IntegralFastIO(std::FILE* stream): fd_(fileno(stream)) {
    ReadToBuffer(); 
  }

  IntegralFastIO(const char* path): fd_(open(path, O_RDONLY)) {
    ReadToBuffer();
  }

public:
  bool ReadOne(T& ret) {
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
    begin_ = 0;
    end_ = read(fd_, buffer_, MAX_BUFFER_SIZE);
  }

private:
  int begin_, end_;
  char buffer_[MAX_BUFFER_SIZE];
  int fd_;
};

} // namespace hermes::fastio

