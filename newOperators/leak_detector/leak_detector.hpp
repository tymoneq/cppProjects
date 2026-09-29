#pragma once

#include <atomic>
#include <cstddef>
class Accountant {
  std::atomic<long long> cur;
  Accountant() : cur{0ll} {}

public:
  Accountant(const Accountant &) = delete;
  Accountant &operator=(const Accountant &) = delete;
  static Accountant &get() {
    static Accountant singleton;
    return singleton;
  }
  void take(std::size_t n) { cur += n; }
  void give_back(std::size_t n) { cur -= n; }
  std::size_t how_much() const { return cur.load(); }
};

void *operator new(std::size_t);
void *operator new[](std::size_t);
void operator delete(void *, std::size_t n) noexcept;
void operator delete[](void *, std::size_t n) noexcept;
