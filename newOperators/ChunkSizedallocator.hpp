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
};