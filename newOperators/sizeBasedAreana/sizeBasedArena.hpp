#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <mutex>

template <class T, std::size_t N> class SizeBasedArena {
  std::mutex m;
  char *p, *cur;
  SizeBasedArena() : p{static_cast<char *>(std::malloc(N * sizeof(T)))} {
    assert(p);
    cur = p;
  }
  SizeBasedArena(const SizeBasedArena &) = delete;
  SizeBasedArena &operator=(const SizeBasedArena &) = delete;

public:
  ~SizeBasedArena() { std::free(p); }
  static auto &get() {
    static SizeBasedArena singleton;
    return singleton;
  }
  void *allocate_one() {
    std::lock_guard _{m};
    auto q = cur;
    cur += sizeof(T);
    return q;
  }
  void *allocate_n(std::size_t n) {
    std::lock_guard _{m};
    auto q = cur;
    cur += n * sizeof(T);
    return q;
  }

  void deallocate_one(void *) noexcept {}
  void deallocate_n(void *) noexcept {}
};