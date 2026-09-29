#include "leak_detector.hpp"
#include <cstddef>
#include <cstdlib>
#include <new>

void *operator new(std::size_t n) {
  void *p = std::malloc(n + sizeof(n)); // do poprawienia

  if (!p)
    throw std::bad_alloc{};

  auto q = static_cast<std::size_t *>(p);
  *q = n; // do poprawienia
  Accountant::get().take(n);
  return q + 1; // do poprawienia
}

void *operator new[](std::size_t n) {
  void *p = std::malloc(n + sizeof(n));
  if (!p)
    throw std::bad_alloc{};

  auto q = static_cast<std::size_t *>(p);
  *q = n; // do poprawienia
  Accountant::get().take(n);
  return q + 1;
}

void operator delete(void *p) noexcept {
  if (!p)
    return;
  auto q = static_cast<std::size_t *>(p) - 1;
  Accountant::get().give_back(*q);
  std::free(q);
}
void operator delete[](void *p) noexcept {
  if (!p)
    return;
  auto q = static_cast<std::size_t *>(p) - 1;
  Accountant::get().give_back(*q);
  std::free(q);
}