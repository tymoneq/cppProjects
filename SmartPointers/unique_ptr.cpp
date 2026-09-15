

#include <cstddef>
#include <type_traits>
#include <utility>
namespace managing_memory_book {

template <class T> struct deleter_pointer_wrapper {
  void (*pf)(T *);
  deleter_pointer_wrapper(void (*pf)(T *)) : pf{pf} {}
  void operator()(T *p) const { pf(p); }
};

template <class T> struct default_deleter {
  void operator()(T *p) const { delete p; }
};
template <class T> struct default_deleter<T[]> {
  void operator()(T *p) const { delete[] p; }
};

template <class T> struct is_deleter_function_candidate : std::false_type {};

template <class T>
struct is_deleter_function_candidate<void (*)(T *)> : std::true_type {};

template <class T>
constexpr auto is_deleter_function_candidate_v =
    is_deleter_function_candidate<T>::value;

// basic unique ptr template
template <class T, class D = default_deleter<T>>
class unique_ptr : std::conditional_t<is_deleter_function_candidate_v<D>,
                                      deleter_pointer_wrapper<T>, D> {
  using deleter_type = std::conditional_t<is_deleter_function_candidate_v<D>,
                                          deleter_pointer_wrapper<T>, D>;
  T *p = nullptr;

public:
  unique_ptr() = default;
  unique_ptr(T *p) : p{p} {}
  unique_ptr(T *p, void (*pf)(T *)) : deleter_type{pf}, p{p} {}
  ~unique_ptr() { (*static_cast<deleter_type *>(this))(p); }

  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  void swap(unique_ptr &other) noexcept {
    using std::swap;
    swap(p, other.p);
  }
  unique_ptr(unique_ptr &&other) noexcept
      : p{std::exchange(other.p, nullptr)} {}

  unique_ptr &operator=(unique_ptr &&other) noexcept {
    unique_ptr{std::move(other)}.swap(*this);
    return *this;
  }

  bool empty() const noexcept { return !p; }
  operator bool() const noexcept { return !empty(); }
  bool operator==(const unique_ptr &other) const noexcept {
    return p == other.p;
  }
  bool operator!=(const unique_ptr &other) const noexcept {
    return !(*this == other);
  }

  T *get() noexcept { return p; }
  const T *get() const noexcept { return p; }

  T &operator*() noexcept { return *p; }
  const T &operator*() const noexcept { return *p; }
  T *operator->() noexcept { return p; }
  const T *operator->() const noexcept { return p; }
};

// specialization for arrays
template <class T, class D>
class unique_ptr<T[], D>
    : std::conditional_t<is_deleter_function_candidate_v<D>,
                         deleter_pointer_wrapper<T>, D> {

  using deleter_type = std::conditional_t<is_deleter_function_candidate_v<D>,
                                          deleter_pointer_wrapper<T>, D>;

  T *p = nullptr;

public:
  unique_ptr() = default;
  unique_ptr(T *p) : p{p} {}
  unique_ptr(T *p, void (*pf)(T *)) : deleter_type{pf}, p{p} {}
  ~unique_ptr() { (*static_cast<deleter_type *>(this))(p); }

  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  void swap(unique_ptr &other) noexcept {
    using std::swap;
    swap(p, other.p);
  }
  unique_ptr(unique_ptr &&other) noexcept
      : p{std::exchange(other.p, nullptr)} {}

  unique_ptr &operator=(unique_ptr &&other) noexcept {
    unique_ptr{std::move(other)}.swap(*this);
    return *this;
  }

  bool empty() const noexcept { return !p; }
  operator bool() const noexcept { return !empty(); }
  bool operator==(const unique_ptr &other) const noexcept {
    return p == other.p;
  }
  bool operator!=(const unique_ptr &other) const noexcept {
    return !(*this == other);
  }

  T *get() noexcept { return p; }
  const T *get() const noexcept { return p; }

  T &operator[](std::size_t n) noexcept { return p[n]; }
  const T &operator[](std::size_t n) const noexcept { return p[n]; }
};

} // namespace managing_memory_book