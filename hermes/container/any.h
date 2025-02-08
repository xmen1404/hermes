#pragma once

#include <glog/logging.h>

#include <type_traits>
#include <typeinfo>
#include <utility>

namespace hermes::container {

class Any {
 public:
  constexpr Any() noexcept
      : data_{nullptr}, Type_{nullptr}, Clone_{nullptr}, Delete_{nullptr} {}

  Any(const Any &rhs) { CopyFrom(rhs); }

  Any(Any &&rhs) noexcept { MoveFrom(std::move(rhs)); }

  template <typename T, std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>,
                                         bool> = true>
  Any(T &&value) {
    AssignValue(std::forward<T>(value));
  }

  ~Any() noexcept { Reset(); }

 public:
  Any &operator=(const Any &rhs) {
    if (this != &rhs) {
      Reset();
      CopyFrom(rhs);
    }
    return *this;
  }

  Any &operator=(Any &&rhs) noexcept {
    if (this != &rhs) {
      Reset();
      MoveFrom(std::move(rhs));
    }
    return *this;
  }

  template <typename T,
            typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>>>
  Any &operator=(T &&value) {
    Reset();
    AssignValue(std::forward<T>(value));
    return *this;
  }

 public:
  template <typename T>
  T Get() const & {
    return T{*static_cast<T *>(data_)};
  }

  template <typename T>
  T Get() && {
    auto ptr = data_;
    data_ = nullptr;
    return T{std::move(*static_cast<T *>(ptr))};
  }

  const std::type_info &Type() const { return Type_ ? Type_() : typeid(void); }

  bool HasValue() const noexcept { return data_ != nullptr; }

  void Reset() noexcept {
    if (data_ != nullptr) {
      Delete_(data_);
      data_ = nullptr;
    }
  }

 private:
  // Helper to set up type-specific function pointers and allocate storage.
  template <typename T>
  void AssignValue(T &&value) {
    using DecayedT = std::decay_t<T>;
    Type_ = []() -> const std::type_info & { return typeid(DecayedT); };
    Clone_ = [](void *data) -> void * {
      return new DecayedT{*static_cast<DecayedT *>(data)};
    };
    Delete_ = [](void *data) { delete static_cast<DecayedT *>(data); };
    data_ = new DecayedT{std::forward<T>(value)};
  }

  // Helper for copy construction/assignment.
  void CopyFrom(const Any &rhs) {
    if (rhs.HasValue()) {
      Type_ = rhs.Type_;
      Clone_ = rhs.Clone_;
      Delete_ = rhs.Delete_;
      data_ = Clone_(rhs.data_);
    }
  }

  // Helper for move construction/assignment.
  void MoveFrom(Any &&rhs) noexcept {
    if (rhs.HasValue()) {
      Type_ = rhs.Type_;
      Clone_ = rhs.Clone_;
      Delete_ = rhs.Delete_;
      data_ = rhs.data_;
      rhs.data_ = nullptr;
    }
  }

 private:
  void *data_{nullptr};
  const std::type_info &(*Type_)();
  void *(*Clone_)(void *);
  void (*Delete_)(void *);
};

template <typename T>
inline static T AnyCast(const Any &any) {
  return any.Get<T>();
}

template <typename T>
inline static T AnyCast(Any &&any) {
  return std::move(any).Get<T>();
}

}  // namespace hermes::container
