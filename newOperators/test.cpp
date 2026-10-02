#include "ChunkSizedallocator.hpp"

#include <chrono>
#include <functional>
#include <print>
#include <utility>
#include <vector>

template <class F, class... Args> auto test(F f, Args &&...args) {
  using namespace std;
  using namespace std::chrono;
  auto pre = high_resolution_clock::now();
  auto res = f(std::forward<Args>(args)...);
  auto post = high_resolution_clock::now();
  return pair{res, post - pre};
}

template <int N> struct dummy {
  char _[N]{};
};
template <int N> auto test_dummy() {
  return std::pair<void *, std::function<void(void *)>>{
      new dummy<N>{}, [](void *p) { delete static_cast<dummy<N> *>(p); }};
}
template <int N, class T> auto test_dummy(T &alloc) {
  return std::pair<void *, std::function<void(void *)>>{
      new (alloc) dummy<N>{},
      [&alloc](void *p) { ::operator delete(p, alloc); }};
}

int main() {
  using namespace std;
  using namespace std::chrono;
  constexpr int N = 100'000;
  using Alloc = ChunkSizedAllocator<N, 32, 62, 128>;
  Alloc chunks;

    auto [r0, dt0] = test([ptrs = std::vector<std::pair<void*, std::function<void(void*)>>>(N * 3)]() mutable {
      // allocation
      for(int i = 0; i != N * 3; i += 3) {
         ptrs[i] = test_dummy<30>();
         ptrs[i + 1] = test_dummy<60>();
         ptrs[i + 2] = test_dummy<100>();
      }
      // cleanup
      for(auto & p : ptrs)
         p.second(p.first);
      return std::size(ptrs);
   });
   auto [r1, dt1] = test([&chunks, ptrs = std::vector<std::pair<void*, std::function<void(void*)>>>(N * 3)]() mutable {
      // allocation
      for(int i = 0; i != N * 3; i += 3) {
         ptrs[i] = test_dummy<30>(chunks);
         ptrs[i + 1] = test_dummy<60>(chunks);
         ptrs[i + 2] = test_dummy<100>(chunks);
      }
      // cleanup
      for(auto & p : ptrs)
         p.second(p.first);
      return std::size(ptrs);
   });
   std::print("Standard version : {}\n", duration_cast<microseconds>(dt0));
   std::print("Chunked version  : {}\n", duration_cast<microseconds>(dt1));
}