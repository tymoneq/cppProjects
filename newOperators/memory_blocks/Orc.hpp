#pragma once

// #define HOMEMADE_VERSION

#include <cstddef>
#include <new>

class Orc {
  char name[4]{'U', 'R', 'G'};
  int strength = 100;
  double smell = 1000.0;

public:
  static constexpr int NB_MAX = 1'000'000;
#ifdef HOMEMADE_VERSION
  void *operator new(std::size_t);
  void *operator new[](std::size_t);
  void operator delete(void *) noexcept;
  void operator delete[](void *) noexcept;

#endif
};
