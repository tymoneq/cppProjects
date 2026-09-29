#include "leak_detector.hpp"
#include <cstddef>
#include <cstdlib>
#include <new>

void *operator new(std::size_t n) {
  void *p = std::malloc(n + sizeof(std::max_align_t));

  if (!p)
    throw std::bad_alloc{};

  new (p) std::size_t{n};
  Accountant::get().take(n);
  return static_cast<std::max_align_t *>(p) + 1;
}

void *operator new[](std::size_t n) {
  void *p = std::malloc(n + sizeof(std::max_align_t));

  if (!p)
    throw std::bad_alloc{};

  new (p) std::size_t{n};
  Accountant::get().take(n);
  return static_cast<std::max_align_t *>(p) + 1;
}

void operator delete(void *p) noexcept {
  if (!p)
    return;
  p = static_cast<std::max_align_t *>(p) - 1;
  Accountant::get().give_back(*static_cast<std::size_t *>(p));
  std::free(p);
}
void operator delete[](void *p) noexcept {
  if (!p)
    return;
  p = static_cast<std::max_align_t *>(p) - 1;
  Accountant::get().give_back(*static_cast<std::size_t *>(p));
  std::free(p);
}