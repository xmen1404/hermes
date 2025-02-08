#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>

struct Job {
  int id{0};
  int next_id{0};
  int prev_id{-1};

  int runtime_sec{0};
  bool visited{false};

  bool IsValid() const noexcept {
    return (id > 0) && (runtime_sec >= 0) && (runtime_sec <= 86400) &&
           (next_id >= 0);
  }
};

struct JobChain {
  int start_id{0};
  int end_id{0};
  int job_cnt{0};
  int tot_runtime_sec{0};

  std::string ToString() const {
    auto TimeFormat = [](int sec) -> std::string {
      auto hr_str = std::to_string(sec / 3600);
      auto min_str = std::to_string((sec % 3600) / 60);
      auto sec_str = std::to_string(sec % 60);

      auto ret = std::string{};
      ret += hr_str.size() > 1 ? hr_str + ":" : "0" + hr_str + ":";
      ret += min_str.size() > 1 ? min_str + ":" : "0" + min_str + ":";
      ret += sec_str.size() > 1 ? sec_str : "0" + sec_str;

      return ret;
    };

    std::ostringstream oss;
    oss << "start_job: " << start_id << '\n'
        << "last_id: " << end_id << '\n'
        << "number_of_jobs: " << job_cnt << '\n'
        << "job_chain_runtime: " << TimeFormat(tot_runtime_sec) << '\n'
        << "average_job_time: " << TimeFormat(tot_runtime_sec / job_cnt) << '\n'
        << "-\n";
    return oss.str();
  }
};

template <int MAX_BUFFER_SIZE = 1 << 13, char DELIMITER = ','>
class CsvReader {
 public:
  CsvReader() {
    buffer_ = new char[MAX_BUFFER_SIZE + 1];
    buffer_[MAX_BUFFER_SIZE] = '\0';
  }

  ~CsvReader() { delete[] buffer_; }

 public:
  bool Init(std::FILE *stream) {
    stream_ = stream;
    ReadToBuffer();

    // ignore CSV header line
    while (IsGood() && NextChar() != '\n') {
    }

    return is_good_;
  }

  Job ReadOne() {
    const auto id = NextInt();
    const auto runtime = NextInt();
    const auto next_id = NextInt();

    return Job{
        .id = id,
        .next_id = next_id,
        .runtime_sec = runtime,
    };
  }

  inline bool IsGood() const noexcept { return is_good_; }

 public:
  static inline bool IsDigit(char c) noexcept {
    return static_cast<unsigned char>(c - '0') < 10;
  }

 private:
  inline int NextInt() noexcept {
    auto ret = 0;

    char c;
    while (IsDigit(c = NextChar())) {
      ret = ret * 10 + (c - '0');
    }
    return ret;
  }

  inline char NextChar() noexcept {
    auto ret = *begin_;
    begin_ += 1;

    if (begin_ >= end_) [[unlikely]]
      ReadToBuffer();

    return ret;
  }

  void ReadToBuffer() {
    *buffer_ = '\0';
    const auto read_size = std::fread(buffer_, 1, MAX_BUFFER_SIZE, stream_);

    begin_ = buffer_;
    end_ = buffer_ + read_size;

    is_good_ = (begin_ != end_);
  }

 private:
  bool is_good_{false};
  char *begin_{nullptr}, *end_{nullptr};
  char *buffer_{nullptr};

  std::FILE *stream_;
};

class JobManager {
 public:
  JobManager() {}

 public:
  bool Init(std::FILE *stream) { return init_succeed_ = InitImpl(stream); }

  // print to STDOUT
  void Report() {
    if (!init_succeed_) {
      std::cout << "Input Malformed";
      return;
    }

    std::cout << "-\n";
    for (const auto &chain : job_chain_list_) {
      const auto output = chain.ToString();
      // TODO: use batch writing to reduce syscall overhead
      write(STDOUT_FILENO, output.c_str(), output.size());
    }
  }

 private:
  bool InitImpl(std::FILE *stream) {
    if (!input_.Init(stream)) return false;

    while (input_.IsGood()) {
      const auto job = input_.ReadOne();

      if (!job.IsValid()) [[unlikely]] {
        // std::cerr << "Malformed Input";
        return false;
      }

      // TODO: consider case there are duplicated job information
      job_mp_.emplace(job.id, job);
    }

    for (auto &p : job_mp_) {
      auto &job = p.second;

      if (!job.next_id) continue;

      if (!job_mp_.count(job.next_id)) {
        // std::cerr << "Next job ID not exists " << job.next_id;
        return false;
      }

      // TODO: check if this case necessary?
      auto &next_job = job_mp_[job.next_id];
      if (next_job.prev_id != -1) {
        // std::cerr << "Duplicate prev_job_id";
        return false;
      }

      next_job.prev_id = job.id;
    }

    for (auto &p : job_mp_) {
      auto &curr_job = p.second;
      if (curr_job.next_id || curr_job.visited) continue;

      // root job
      auto chain = JobChain{};

      chain.start_id = curr_job.id;
      chain.end_id = curr_job.id;
      chain.job_cnt = 1;
      chain.tot_runtime_sec = curr_job.runtime_sec;

      curr_job.visited = true;
      while (curr_job.prev_id != -1) {
        curr_job = job_mp_[curr_job.prev_id];

        // TODO: check if this case is necessary
        if (curr_job.visited) [[unlikely]] {
          // std::cerr << "Job chain has circular loop";
          return false;
        }

        curr_job.visited = true;
        chain.start_id = curr_job.id;
        chain.tot_runtime_sec += curr_job.runtime_sec;
        chain.job_cnt += 1;
      }

      job_chain_list_.emplace_back(chain);
    }

    return true;
  }

 private:
  bool init_succeed_{true};
  std::unordered_map<int, Job> job_mp_;
  std::vector<JobChain> job_chain_list_;

  CsvReader<> input_;
};
