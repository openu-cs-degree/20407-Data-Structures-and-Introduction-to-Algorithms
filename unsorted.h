#ifndef UNSORTED_HEAP_H
#define UNSORTED_HEAP_H

#include <optional>
#include <functional>
#include <type_traits>

#include "mergeable_heap.h"

/**
 * @class UnsortedLinkedHeap
 *
 * @brief A mergeable heap data structure implementation using an unsorted linked list.
 *
 * @details This mergeable heap is implemented using an unsorted linked list. The heap is
 * maintained in an unsorted order, with pointers to the tail and the minimum nodes in
 * the list to allow for constant time insertion, merge and extraction of the minimum key.
 *
 * @tparam T The type of the elements stored in the heap. `T` must be movable, as it is
 * moved into the heap in the insert method.
 */
template <typename T>
class UnsortedLinkedHeap : public MergeableHeap<T>
{
private:
  struct Node
  {
    T key;      ///< The key stored in the node.
    Node *next; ///< A pointer to the next node in the linked list.
    Node *prev; ///< A pointer to the previous node in the linked list.

    /**
     * @brief Constructs a new node with the given key.
     *
     * @param key The key to store in the node. This key is moved into the node.
     */
    constexpr Node(T key) : key(std::move(key)), next(nullptr), prev(nullptr) {}

    /**
     * @brief Default destructor.
     *
     * The deletion of the next / prev nodes is handled by the heap.
     */
    constexpr ~Node() = default;
  };

private:
  /**
   * @brief Constructs a new heap with the given key.
   *
   * The time complexity of this operation is O(1).
   *
   * @param key The key to store in the heap. This key is moved into the heap.
   */
  constexpr UnsortedLinkedHeap(T key) : head(new Node(std::move(key))), tail(head), min(head) {}

public:
  /**
   * @brief Constructs a new empty heap.
   *
   * This constructor initializes the heap to be empty. The head, tail and min are set to
   * `nullptr`.
   *
   * The time complexity of this operation is O(1).
   */
  constexpr UnsortedLinkedHeap() : head(nullptr), tail(nullptr), min(nullptr) {}

  /**
   * @brief Destroys the heap.
   *
   * The destructor deletes all nodes in the linked list.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the
   * heap.
   */
  constexpr ~UnsortedLinkedHeap()
  {
    for (Node *current = head; current != nullptr;)
    {
      Node *next = current->next;
      delete current;
      current = next;
    }
  }

  /**
   * @brief Inserts a key into the heap.
   *
   * This function inserts a new key into the heap. The key is inserted at the end of the
   * linked list, and the minimum key is updated if the new key is smaller than the current
   * minimum.
   *
   * The time complexity of this operation is O(1).
   *
   * @param key The key to insert into the heap. This key is moved into the heap.
   */
  constexpr void insert(T key) override
  {
    Node *node = new Node(std::move(key));
    if (head == nullptr)
    {
      head = tail = node;
    }
    else
    {
      tail->next = node;
      node->prev = tail;
      tail = node;
    }
    if (min == nullptr || node->key < min->key)
    {
      min = node;
    }
  }

  /**
   * @brief Returns the minimum key in the heap.
   *
   * This method returns a reference to the minimum key in the heap, or `std::nullopt`
   * if the heap is empty. The reference is wrapped in a `std::reference_wrapper` to
   * allow it to be used in contexts where a reference cannot be used.
   *
   * The time complexity of this operation is O(1), because the minimum key is stored
   * in the `min` pointer, which is always available at constant time.
   *
   * @return A reference to the minimum key, or `std::nullopt` if the heap is empty.
   */
  constexpr std::optional<std::reference_wrapper<const T>> minimum() const override
  {
    if (min == nullptr)
    {
      return std::nullopt;
    }
    return std::cref(min->key);
  }

  /**
   * @brief Removes and returns the minimum key in the heap.
   *
   * This method removes the minimum key from the heap and returns it. If the heap is
   * empty, the method returns `std::nullopt`.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the
   * heap, because the new minimum key must be searched for in the entire linked list.
   *
   * @return The minimum key in the heap, or `std::nullopt` if the heap is empty.
   */
  constexpr std::optional<T> extract_min() override
  {
    if (min == nullptr)
    {
      return std::nullopt;
    }

    T key = std::move(min->key);
    Node *next = min->next;
    Node *prev = min->prev;

    if (prev != nullptr) // min is not the head
    {
      prev->next = next;
    }
    if (next != nullptr) // min is not the tail
    {
      next->prev = prev;
    }

    if (min == head)
    {
      head = next; // update head
    }
    if (min == tail)
    {
      tail = prev; // update tail
    }

    delete min;
    min = nullptr;

    update_min(); // O(n)

    return key;
  }

  /**
   * @brief Merges another heap into this heap.
   *
   * This function merges another heap into this heap. The other heap is emptied in the
   * process, and its nodes are moved into this heap. The minimum key is updated if the
   * new heap contains a smaller key than the current minimum.
   *
   * @note The other heap is left empty after the merge.
   *
   * The time complexity of this operation is O(1), because the other heap is merged
   * into this heap by concatenating the tail of the other heap with the head of this heap.
   *
   * @param other The heap to merge into this heap.
   */
  constexpr void merge(MergeableHeap<T> &other) override
  {
    UnsortedLinkedHeap<T> &other_heap = static_cast<UnsortedLinkedHeap<T> &>(other);

    if (other_heap.head == nullptr)
    {
      return;
    }

    if (head == nullptr) // this heap is empty
    {
      head = other_heap.head;
      tail = other_heap.tail;
      min = other_heap.min;
    }
    else // this heap is not empty, concatenate the two heaps
    {
      tail->next = other_heap.head;
      other_heap.head->prev = tail;
      tail = other_heap.tail;

      if (other_heap.min->key < min->key)
      {
        min = other_heap.min;
      }
    }

    other_heap.head = other_heap.tail = other_heap.min = nullptr;
  }

  /**
   * @brief Prints the heap.
   *
   * This function prints the keys in the heap in the order they are stored in the linked
   * list. The keys are separated by commas and followed by a period.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the
   * heap.
   */
  void print() const override
  {
    for (Node *current = head; current != nullptr; current = current->next)
    {
      std::cout << current->key << ", ";
    }
    std::cout << "\b\b.\n";
  }

private:
  Node *head; ///< A pointer to the first node in the linked list.
  Node *tail; ///< A pointer to the last node in the linked list.
  Node *min;  ///< A pointer to the node with the minimum key.

  void update_min()
  {
    min = head;
    if (head == nullptr)
    {
      return;
    }
    for (Node *current = head->next; current != nullptr; current = current->next)
    {
      if (current->key < min->key)
      {
        min = current;
      }
    }
  }
}; // class UnsortedLinkedHeap

#endif // SORTED_HEAP_H