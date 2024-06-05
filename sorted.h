#ifndef SORTED_HEAP_H
#define SORTED_HEAP_H

#include <functional>

#include "mergeable_heap.h"

/**
 * @class SortedLinkedHeap
 *
 * @brief A mergeable heap data structure implementation using a sorted linked list.
 *
 * @details This mergeable heap is implemented using a sorted linked list. The heap is
 * maintained in a sorted order, where the minimum key is always at the head of the list.
 *
 * @tparam T The type of the elements stored in the heap. `T` must be movable, as it is
 * moved into the heap in the insert method.
 */
template <typename T>
class SortedLinkedHeap : public MergeableHeap<T>
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
  constexpr SortedLinkedHeap(T key) : head(new Node(std::move(key))) {}

public:
  /**
   * @brief Constructs a new empty heap.
   *
   * This constructor initializes the heap to be empty. The head is set to `nullptr`.
   *
   * The time complexity of this operation is O(1).
   */
  constexpr SortedLinkedHeap() : head(nullptr) {}

  /**
   * @brief Destroys the heap.
   *
   * The destructor deletes all nodes in the linked list.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the heap.
   */
  constexpr ~SortedLinkedHeap()
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
   * This function inserts a new key into the heap. The key is inserted at the correct
   * position in the linked list to maintain the sorted order of the heap.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the heap.
   *
   * @param key The key to insert into the heap. This key is moved into the heap.
   */
  constexpr void insert(T key) override
  {
    SortedLinkedHeap<T> temp(std::move(key));
    merge(temp);
  }

  /**
   * @brief Returns the minimum key in the heap.
   *
   * This method returns a reference to the minimum key in the heap, or `std::nullopt`
   * if the heap is empty. The reference is wrapped in a `std::reference_wrapper` to
   * allow it to be used in contexts where a reference cannot be used.
   *
   * The time complexity of this operation is O(1), because the minimum key is stored
   * in the `head` pointer, which is always available at constant time.
   *
   * @return A reference to the minimum key, or `std::nullopt` if the heap is empty.
   */
  constexpr std::optional<std::reference_wrapper<const T>> minimum() const override
  {
    if (head == nullptr)
    {
      return std::nullopt;
    }
    return std::cref(head->key);
  }

  /**
   * @brief Removes and returns the minimum key in the heap.
   *
   * This method removes the minimum key from the heap and returns it. If the heap is
   * empty, the method returns `std::nullopt`.
   *
   * The time complexity of this operation is O(1), because the minimum key is stored
   * in the `head` pointer, which is always available at constant time.
   *
   * @return The minimum key in the heap, or `std::nullopt` if the heap is empty.
   */
  constexpr std::optional<T> extract_min() override
  {
    Node *min_node = head;
    if (min_node == nullptr)
    {
      return std::nullopt;
    }

    head = head->next;
    if (head != nullptr) // the heap is not empty
    {
      head->prev = nullptr;
    }

    T key = std::move(min_node->key); // extract the key before deleting the node

    min_node->next = nullptr;
    delete min_node;

    return key;
  }

  /**
   * @brief Merges another heap into this heap.
   *
   * This method merges another heap into this heap. The other heap is emptied in the
   * process, and its nodes are inserted into this heap in sorted order.
   *
   * @note The other heap is left empty after the merge.
   *
   * The time complexity of this operation is O(n + m), where n is the number of nodes
   * in this heap and m is the number of nodes in the other heap.
   *
   * @param other The heap to merge into this heap.
   */
  constexpr void merge(MergeableHeap<T> &other) override
  {
    SortedLinkedHeap<T> &other_heap = static_cast<SortedLinkedHeap<T> &>(other);

    if (head == nullptr)
    {
      std::swap(head, other_heap.head);
      return;
    }
    if (other_heap.head == nullptr)
    {
      return;
    }

    // Initialize pointers
    Node *current = head;
    Node *otherCurrent = other_heap.head;
    Node *prev = nullptr;

    // Iterate through both lists
    while (current != nullptr && otherCurrent != nullptr)
    {
      if (current->key < otherCurrent->key)
      {
        // Insert otherCurrent node after prev
        prev = current;
        current = current->next;
      }
      else
      {
        // Insert otherCurrent node before current
        Node *nextOtherCurrent = otherCurrent->next;
        if (prev == nullptr)
        {
          head = otherCurrent;
        }
        else
        {
          prev->next = otherCurrent;
        }
        otherCurrent->prev = prev;
        otherCurrent->next = current;
        prev = otherCurrent;
        otherCurrent = nextOtherCurrent;
      }
    }

    // Append remaining nodes from either list
    if (current == nullptr)
    {
      // There are remaining nodes in the other list
      prev->next = otherCurrent;
      if (otherCurrent != nullptr)
      {
        otherCurrent->prev = prev;
      }
    }

    other_heap.head = nullptr;
  }

  /**
   * @brief Prints the heap.
   *
   * This function prints the keys in the heap in sorted order. The keys are separated by
   * commas and followed by a period.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the
   * heap.
   */
  void print() const override
  {
    if (head == nullptr)
    {
      std::cout << "empty.";
      return;
    }

    for (Node *current = head; current != nullptr; current = current->next)
    {
      std::cout << current->key << ", ";
    }
    std::cout << "\b\b.";
  }

private:
  Node *head; ///< A pointer to the first node in the linked list.
}; // class SortedLinkedHeap

#endif // SORTED_HEAP_H