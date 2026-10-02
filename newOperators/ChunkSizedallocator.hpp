#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <utility>
#include <vector>

template <class T, std::same_as<T>... Ts>
constexpr std::array<T, sizeof...(Ts) + 1> make_array(T n, Ts... ns) {
  return {n, ns...};
}

constexpr bool is_power_of_two(std::integral auto n) {
  return n && ((n & (n - 1)) == 0);
}

class integral_value_too_big {};

constexpr auto next_power_of_two(std::integral auto n) {
  constexpr auto upper_limit = std::numeric_limits<decltype(n)>::max();
  for (; n != upper_limit && !is_power_of_two(n); ++n)
    ;
  if (!is_power_of_two(n))
    throw integral_value_too_big{};
  return n;
}

template <class T> constexpr bool is_sorted(const T &c) {
  return std::is_sorted(std::begin(c), std::end(c));
}

template <int N, auto... Sz> class ChunkSizedAllocator {
  static_assert(is_sorted(make_array(Sz...)));
  static_assert(sizeof...(Sz) > 0);
  static_assert(((Sz >= sizeof(std::max_align_t)) && ...));
  static_assert(N > 0);
  static constexpr unsigned long long sizes[]{next_power_of_two(Sz)...};

  using raw_ptr = void *;
  raw_ptr blocks[sizeof...(Sz)];
  int cur[sizeof...(Sz)]{};

  bool within_block(void *p, int i) {
    void *b = blocks[i];
    void *e = static_cast<char *>(b) + N * sizes[i];
    return p == b || (std::less{}(b, p) && std::less{}(p, e));
  }

  std::mutex m;

public:
  ChunkSizedAllocator(const ChunkSizedAllocator &) = delete;
  ChunkSizedAllocator &operator=(const ChunkSizedAllocator &) = delete;

  ChunkSizedAllocator() {
    int i = 0;
    for (auto sz : sizes)
      blocks[i++] = std::malloc(N * sz);
    assert(std::none_of(std::begin(blocks), std::end(blocks),
                        [](auto p) { return !p; }));
  }

  ~ChunkSizedAllocator() {
    for (auto p : blocks)
      std::free(p);
  }

  auto allocate(std::size_t n) {
    using std::size;
    for (std::size_t i = 0; i != size(sizes); ++i) {
      if (n < sizes[i]) {
        std::lock_guard _{m};
        if (cur[i] < N) {
          void *p = static_cast<char *>(blocks[i]) + cur[i] * sizes[i];
          ++cur[i];
          return p;
        }
      }
    }

    return ::operator new(n);
  }

  void deallocate(void *p) {
    using std::size;
    for (std::size_t i = 0; i != size(sizes); ++i) {
      if (within_block(p, i)) {
        return;
      }
    }

    ::operator delete(p);
  }
};

template <int N, auto... Sz>
void *operator new(std::size_t n, ChunkSizedAllocator<N, Sz...> &chunks) {
  return chunks.allocate(n);
}

template <int N, auto... Sz>
void *operator new[](std::size_t n, ChunkSizedAllocator<N, Sz...> &chunks) {
  return chunks.allocate(n);
}

template <int N, auto... Sz>
void operator delete(void *p, ChunkSizedAllocator<N, Sz...> &chunks) {
  return chunks.deallocate(p);
}
template <int N, auto... Sz>
void operator delete[](void *p, ChunkSizedAllocator<N, Sz...> &chunks) {
  return chunks.deallocate(p);
}

