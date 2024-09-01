/**
  @file main.cpp
  @author Yehonatan Simian
  @date June 2024

  \verbatim
  +-----------------------------------------------------------------------------------+
  |                        20407 - Data Structures - Maman 13                         |
  |                                                                                   |
  |                 "One must imagine Sisyphus happy." - Albert Camus                 |
  +-----------------------------------------------------------------------------------+
  \endverbatim

  @section DESCRIPTION

  This project includes three implementations of a mergeable heap data structure:
  1. UnsortedLinkedHeap: A mergeable heap implemented using an unsorted linked list.
  2. SortedLinkedHeap: A mergeable heap implemented using a sorted linked list.
  3. LazyHeap: A mergeable heap implemented using a lazy binomial heap.

  The project also includes a Game class that allows the user to interact with the heaps
  through a command-line interface. The user can create, insert, extract, merge, print,
  and sort heaps.

  Complexity table:
  | Operation   | UnsortedLinkedHeap | SortedLinkedHeap | LazyBinomialHeap   |
  |-------------|--------------------|------------------|--------------------|
  | MAKE-HEAP   | O(1)               | O(1)             | O(1)               |
  | INSERT      | O(1)               | O(n)             | O(1)               |
  | MINIMUM     | O(1)               | O(1)             | O(1)               |
  | EXTRACT-MIN | O(n)               | O(1)             | O(log n) amortized |
  | UNION       | O(1)               | O(n+m)           | O(1)               |

  @note My implementations of the mergeable heap are all constexpr-friendly, but do not
  handle runtime memory allocation failures for the sake of simplicity.

  @section USAGE

  The program is very self explanatory. Better to run it and see for yourself!

  @section COMPILATION

  To compile this project, I have used the following command:

  g++ -std=c++23 -Wall -Wextra -Werror -Wpedantic -o main main.cpp

  @note My implementations use C++23, and were tested using gcc 13.1.0.

  @copyright All rights reserved (c) Yehonatan Simian 2024
*/

#include "game.hpp"

int main()
{
  Game game{};
  game.run();
}