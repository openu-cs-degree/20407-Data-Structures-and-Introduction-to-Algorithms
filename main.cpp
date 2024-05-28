#include "heap.h"

constexpr auto foo()
{
  MergeableHeap<int> a{};
  a.insert(1);
  a.insert(3);
  a.insert(2);
  a.insert(3);
  a.extract_min();
  return a.extract_min();
}

int main()
{
  constexpr auto res = foo();
  return *res; // 2
}