#include "lazy.h"

// g++ -std=c++2b -o example example.cpp -Werror -Wall -Wextra -Wpedantic -O3

// objdump -d ./example
// 0000000000001040 <main>:
//     1040:       f3 0f 1e fa             endbr64
//     1044:       b8 02 00 00 00          mov    $0x2,%eax
//     1049:       c3                      ret
//     104a:       66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)

constexpr auto foo()
{
  LazyBinomialHeap<int> a{};
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