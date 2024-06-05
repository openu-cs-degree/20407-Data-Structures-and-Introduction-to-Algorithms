/**
  @file mergeable_heap.h
  @author Yehonatan Simian
  @date May 2024

  +-----------------------------------------------------------------------------------+
  |                        20407 - Data Structures - Maman 13                         |
  |                                                                                   |
  |                 "One must imagine Sisyphus happy." - Albert Camus                 |
  +-----------------------------------------------------------------------------------+

  @section DESCRIPTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  This file contains the interface of a mergeable heap data structure, which is a
  type of heap that supports the following operations:
  1. MAKE-HEAP creates a new (empty) heap.
  2. INSERT inserts a new element into the heap.
  3. MINIMUM returns the element with the minimum key in the heap.
  4. EXTRACT-MIN removes and returns the element with the minimum key in the heap.
  5. UNION merges two heaps into a single heap.

  @note The interface is constexpr-friendly.
  @note My implementation will not handle memory allocation failures for simplicity.

  @section COMPILATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  To compile this project, I have used the following command:

  g++ -std=c++23 -Wall -Wextra -Werror -Wpedantic -o main main.cpp

  @note The mergeable heap is implemented using C++23, and was tested using gcc 13.1.0.

  @copyright All rights reserved (c) Yehonatan Simian 2024 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

#ifndef MERGEABLE_HEAP_H
#define MERGEABLE_HEAP_H

#include <optional>
#include <iostream>

template <typename T>
class MergeableHeap
{
public:
  /**
   * Constructs a new empty heap.
   */
  constexpr MergeableHeap() = default;

  /**
   * Destroys the heap.
   */
  constexpr virtual ~MergeableHeap() = default;

  /**
   * @brief Deleted special member functions.
   *
   * `MergeableHeap` objects should not be copied or moved.
   */
  constexpr MergeableHeap(const MergeableHeap &) = delete;
  constexpr MergeableHeap &operator=(const MergeableHeap &) = delete;
  constexpr MergeableHeap(MergeableHeap &&) = delete;
  constexpr MergeableHeap &operator=(MergeableHeap &&) = delete;

  /**
   * Inserts a key into the heap.
   */
  constexpr virtual void insert(T key) = 0;

  /**
   * Returns the minimum key in the heap.
   */
  constexpr virtual std::optional<std::reference_wrapper<const T>> minimum() const = 0;

  /**
   * Removes and returns the minimum key in the heap.
   */
  constexpr virtual std::optional<T> extract_min() = 0;

  /**
   * Merges another heap into this heap.
   */
  constexpr virtual void merge(MergeableHeap<T> &other) = 0;

  /**
   * Prints the heap.
   */
  virtual void print() const = 0;

  /**
   * @brief Sorts the heap in ascending order.
   *
   * This function extracts the minimum key from the heap and prints it until the heap
   * is empty. The extracted keys are then inserted into a temporary heap, which is
   * then merged back into the original heap.
   *
   * Complexity depends on the implementation of the heap:
   * O(extract_min) * O(insert) + O(merge).
   */
  virtual void sort() = 0;

protected:
  /**
   * @brief Sorts the heap using a temporary heap.
   *
   * This function should be called by the `sort` function of the derived class
   * in the following way: `this->MergeableHeap<T>::sort(DerivedHeap<T>{})`.
   */
  void sort(MergeableHeap<T> &&temp)
  {
    while (auto key = extract_min())
    {
      temp.insert(std::move(*key));
    }
    merge(temp);
  }
};

#endif // MERGEABLE_HEAP_H