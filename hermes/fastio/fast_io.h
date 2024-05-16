#include <cstdio>
#include <type_traits>

namespace hermes::fastio {

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
class IntegralFastIO {
public:
  IntegralFastIO() = delete;
  IntegralFastIO(std::FILE* stream): stream_(stream) {
    ReadToBuffer();
    
    // skip whitespace
    while (begin_ != end_) {
      if (buffer_[begin_] >= '0' || buffer_[begin_] <= '9')
        break;
      begin_ += 1;
      if (begin_ == end_)
        ReadToBuffer();
    }
  }

public:
  bool ReadOne(T& ret) {
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

    // skip whitespace
    while (begin_ != end_) {
      if (buffer_[begin_] >= '0' && buffer_[begin_] <= '9')
        break;
      begin_ += 1;
      if (begin_ == end_)
        ReadToBuffer();
    } 
    
    return true;
  }

private:
  void ReadToBuffer() {
    const auto read_cnt = std::fread(buffer_, 1, MAX_BUFFER_SIZE, stream_);
    
    begin_ = 0;
    end_ = read_cnt;
  }

private:
  static constexpr int MAX_BUFFER_SIZE = 1<<12;

private:
  int begin_, end_;
  char buffer_[MAX_BUFFER_SIZE];
  std::FILE* stream_;
};

} // namespace hermes::fastio

