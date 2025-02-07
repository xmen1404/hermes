#pragma once

#include <atomic>
#include <memory>
#include <type_traits>

namespace hermes::container {

struct NullOptional {};

constexpr NullOptional nullopt{};

template <typename T> class Optional {
  template <typename U> friend class Optional;

public:
  Optional() noexcept {}
  Optional(NullOptional) {}

  template <typename U,
            typename std::enable_if<std::is_constructible<T, const U &>::value,
                                    bool>::type = true>
  Optional(const Optional<U> &rhs) {
    if (rhs.has_value_) {
      has_value_ = true;
      new (reinterpret_cast<T *>(data_))
          T{*reinterpret_cast<const U *>(rhs.data_)};
    }
  }

  template <typename U,
            typename std::enable_if<std::is_constructible<T, U &&>::value,
                                    bool>::type = true>
  Optional(Optional<U> &&rhs) {
    if (rhs.has_value_) {
      has_value_ = true;
      new (reinterpret_cast<T *>(data_))
          T{std::move(*reinterpret_cast<U *>(rhs.data_))};
    }
  }

  template <typename... Args,
            typename std::enable_if<std::is_constructible<T, Args...>::value,
                                    bool>::type = true>
  Optional(Args... params) {
    has_value_ = true;
    new (reinterpret_cast<T *>(data_)) T{std::forward<Args>(params)...};
  }

  template <typename U = T,
            typename std::enable_if<std::is_constructible<T, U &&>::value,
                                    bool>::type = true>
  Optional(U &&value) {
    has_value_ = true;
    new (reinterpret_cast<T *>(data_)) T{std::forward<U>(value)};
  }

  ~Optional() noexcept { Reset(); }

  template <typename U,
            typename std::enable_if<std::is_constructible<T, const U &>::value,
                                    bool>::type = true>
  Optional<T> &operator=(const Optional<U> &rhs) {
    Reset();

    if (rhs.has_value_) {
      has_value_ = true;
      new (reinterpret_cast<T *>(data_))
          T{*reinterpret_cast<const U *>(rhs.data_)};
    }

    return *this;
  }

  template <typename U,
            typename std::enable_if<std::is_constructible<T, U &&>::value,
                                    bool>::type = true>
  Optional<T> &operator=(Optional<U> &&rhs) {
    Reset();

    if (rhs.has_value_) {
      has_value_ = true;
      new (reinterpret_cast<T *>(data_))
          T{std::move(*reinterpret_cast<U *>(rhs.data_))};
    }

    return *this;
  }

  template <typename U = T,
            typename std::enable_if<std::is_constructible<T, U &&>::value,
                                    bool>::type = true>
  Optional<T> &operator=(U &&value) {
    Reset();

    has_value_ = true;
    new (reinterpret_cast<T *>(data_)) T{std::forward<U>(value)};

    return *this;
  }

  friend bool operator==(const Optional<T> &opt, NullOptional) noexcept {
    return !opt.HasValue();
  }

  friend bool operator==(NullOptional, const Optional<T> &opt) noexcept {
    return !opt.HasValue();
  }

  friend bool operator!=(const Optional<T> &opt, NullOptional) noexcept {
    return opt.HasValue();
  }

  friend bool operator!=(NullOptional, const Optional<T> &opt) noexcept {
    return opt.HasValue();
  }

public:
  constexpr T &Value() &noexcept { return *reinterpret_cast<T *>(data_); }

  constexpr const T &Value() const &noexcept {
    return *reinterpret_cast<T *>(data_);
  }

  constexpr T &&Value() &&noexcept {
    return std::move(*reinterpret_cast<T *>(data_));
  }

  bool HasValue() const noexcept { return has_value_; }

  void Reset() noexcept {
    if (has_value_) {
      reinterpret_cast<T *>(data_)->~T();
    }
    has_value_ = false;
  }

private:
  bool has_value_{false};
  char data_[sizeof(T)];
};

} // namespace hermes::container
