#include "Orc.hpp"
#ifdef HOMEMADE_VERSION

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <mutex>

class Tribe {
  std::mutex m;
  char *p, *cur;
  Tribe() : p{static_cast<char *>(std::malloc(Orc::NB_MAX * sizeof(Orc)))} {
    assert(p);
    cur = p;
  }
  Tribe(const Tribe &) = delete;
  Tribe &operator=(const Tribe &) = delete;

public:
  ~Tribe() { std::free(p); }
  static auto &get() {
    static Tribe singleton;
    return singleton;
  }
  void *allocate() {
    std::lock_guard _{m};
    auto q = cur;
    cur += sizeof(Orc);
    return q;
  }
  void deallocate(void *) noexcept {}
};

void *Orc::operator new(std::size_t) { return Tribe::get().allocate(); }
void *Orc::operator new[](std::size_t) { assert(false); }
void Orc::operator delete(void *p) noexcept { Tribe::get().deallocate(p); }
void Orc::operator delete[](void *) noexcept { assert(false); }
 #endif