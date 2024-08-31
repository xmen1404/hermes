#include <string>
#include <optional>
#include <unordered_map>

#include <glog/logging.h>

namespace hermes::config {

class Config {
 public:
  Config() {}

  bool Has(const std::string& key) const noexcept {
    return nodes_.count(key);
  }

  template <typename T>
  T Get(const std::string& key) const noexcept {  
  }

 private:
  std::unordered_map<std::string, std::string> nodes_;
};

} // hermes::config
