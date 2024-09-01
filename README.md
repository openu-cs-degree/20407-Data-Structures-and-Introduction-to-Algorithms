# Overview

This project provides an implementation of the **mergeable heap** interface through [three](#implementation) distinct approaches, each crafted with attention to detail and professionalism. The implementations leverage modern C++ features such as [compile-time heap allocations](http://wg21.link/P0784R1) and [constexpr virtual functions](https://wg21.link/P1064) to ensure efficiency and maintainability.

Additionally, a user interface is [provided](#user-interface) to facilitate interaction with these data structures.

## Mergeable Heaps

In computer science, a **mergeable heap** (also called a meldable heap) is an abstract data type that supports a merge operation. Mergeable heaps are particularly useful in applications where dynamic sets of elements need to be managed efficiently, such as in Dijkstra's algorithm.

Mergeable heaps support five main operations: MAKE-HEAP, INSERT, MINIMUM, EXTRACT-MIN, and UNION. The MINIMUM and EXTRACT-MIN operations can be symmetrically replaced with MAXIMUM and EXTRACT-MAX. The operations' time complexity are described in the [Documentation](#documentation) section.

The mergeable heap interface is declared in [mergeable_heap.h](https://github.com/yonisimian/mergeable-heap/blob/main/mergeable_heap.h).

## Implementation

The project includes the following implementations of the mergeable heap interface:

- [**unsorted.h**](https://github.com/yonisimian/mergeable-heap/blob/main/unsorted.h): An implementation using **unsorted linked lists**.
- [**sorted.h**](https://github.com/yonisimian/mergeable-heap/blob/main/sorted.h): An implementation using **sorted linked lists**.
- [**lazy.h**](https://github.com/yonisimian/mergeable-heap/blob/main/lazy.h): An implementation using **lazy binomial heaps**.

Each implementation has been carefully designed to utilize modern C++ features, ensuring both performance and code clarity.

## User Interface

A user interface is provided via the [game.hpp](https://github.com/yonisimian/mergeable-heap/blob/main/game.hpp) file. I tried to make it visually appealing and also fun to interact with 'cause I'm a based sigma that can't take those rizzless programmers who think professionalism and dullness go hand by hand.

To run the program, just include the game header and use the following code:

```cpp
#include "game.hpp"

int main() {
    Game game{};
    game.run();
}
```

The above code is available in [main.cpp](https://github.com/yonisimian/mergeable-heap/blob/main/main.cpp). It can be compiled using the following command:

```sh
g++ -std=c++23 -Wall -Wextra -Werror -Wpedantic -o main main.cpp
```

I recommend using GCC 13.1.0 or later versions for compilation.


## Documentation

The code files are thoroughly documented. In addition to the in-code comments, Doxygen-generated documentation is available:

- [docs.pdf](https://github.com/yonisimian/mergeable-heap/blob/main/docs.pdf)
- [docs-compact.pdf](https://github.com/yonisimian/mergeable-heap/blob/main/docs-compact.pdf)

Below is a complexity table that summarizes the performance characteristics of the different heap implementations:

|  Operation  | UnsortedLinkedHeap | SortedLinkedHeap |  LazyBinomialHeap  |
|-------------|--------------------|------------------|--------------------|
|  MAKE-HEAP  |        O(1)        |       O(1)       |        O(1)        |
|   INSERT    |        O(1)        |       O(n)       |        O(1)        |
|   MINIMUM   |        O(1)        |       O(1)       |        O(1)        |
| EXTRACT-MIN |        O(n)        |       O(1)       | O(log n) amortized |
|    UNION    |        O(1)        |      O(n+m)      |        O(1)        |

> **Note**: The lazy binomial heap data structure was implemented accidentally, as it was not a homework requirement according to the forum. However, it has been retained due to its elegance and efficiency.

## Example

The file [example.cpp](https://github.com/yonisimian/mergeable-heap/blob/main/example.cpp) provides a concise example demonstrating the usage of the mergeable heap, as well as a proof of the implementation's efficiency.

By utilizing the mergeable heap in a `constexpr` context, all heap allocations and deallocations are performed at compile-time, resulting in optimized performance and reduced runtime overhead.

---

Yehonatan Simian, 2024 Ⓒ <yonisimian@gmail.com>
