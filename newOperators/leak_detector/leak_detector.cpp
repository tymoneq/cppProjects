#include "leak_detector.hpp"
#include <cstddef>
#include <cstdlib>
#include <new>

void *operator new(std::size_t n) {
  void *p = std::malloc(n);

  if (!p)
    throw std::bad_alloc{};

  Accountant::get().take(n);
  return p;
}

void *operator new[](std::size_t n) {
  void *p = std::malloc(n);

  if (!p)
    throw std::bad_alloc{};

  Accountant::get().take(n);
  return p;
}

void operator delete(void *p, std::size_t n) noexcept {
  if (!p)
    return;
  Accountant::get().give_back(n);
  std::free(p);
}
void operator delete[](void *p,std::size_t n) noexcept {
  if (!p)
    return;
  Accountant::get().give_back(n);
  std::free(p);
}