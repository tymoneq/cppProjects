#include "Orc.hpp"
#ifdef HOMEMADE_VERSION
#include "sizeBasedArena.hpp"
#include <cassert>
#include <cstddef>
#include <cstdlib>

using Tribe = SizeBasedArena<Orc, Orc::NB_MAX>;

void *Orc::operator new(std::size_t) { return Tribe::get().allocate_one(); }
void *Orc::operator new[](std::size_t n) {
  return Tribe::get().allocate_n(n / sizeof(Orc));
}
void Orc::operator delete(void *p) noexcept { Tribe::get().deallocate_one(p); }
void Orc::operator delete[](void *p) noexcept { Tribe::get().deallocate_n(p); }
#endif