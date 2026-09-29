#include "leak_detector.hpp"
#include <iostream>

int main() {
  auto pre = Accountant::get().how_much();

  {
    int *p = new int{3};
    int *q = new int[10];
    delete p;
  }
  auto post = Accountant::get().how_much();
  if (post != pre) {
    std::cout << "Leaked " << (post - pre) << " bytes\n";
  }
  return 0;
}