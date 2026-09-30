#include <chrono>
#include <utility>
#include "Orc.hpp"
#include <print>
#include <vector>


template <class F, class... Args> auto test(F f, Args &&...args) {
  using namespace std;
  using namespace std::chrono;
  auto pre = high_resolution_clock::now();
  auto res = f(std::forward<Args>(args)...);
  auto post = high_resolution_clock::now();
  return pair{res, post - pre};
}


int main() {
  using namespace std;
  using namespace std::chrono;
#ifdef HOMEMADE_VERSION
  print("HOMEMADE_VERSION\n");
#else
  print("STANDARD LIBRARY VERSION\n");
#endif

  vector<Orc *> orcs;
  auto [r0, dt0] = test([&orcs] {
    for (int i = 0; i != Orc::NB_MAX; ++i)
      orcs.push_back(new Orc);
    return size(orcs);
  });

  //  Masakra

  auto [r1, dt1] = test([&orcs] {
    for (auto p : orcs)
      delete p;
    return size(orcs);
  });

  print("Construction: {} orcs in {}\n", size(orcs),
        duration_cast<microseconds>(dt0));
  print("Destruction: {} orcs in {}\n", size(orcs),
        duration_cast<microseconds>(dt1));
}