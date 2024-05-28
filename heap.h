/**
  @file heap.h
  @author Yehonatan Simian
  @date May 2024

  +-----------------------------------------------------------------------------------+
  |                        20407 - Data Structures - Maman 13                         |
  |                                                                                   |
  |                 "One must imagine Sisyphus happy." - Albert Camus                 |
  +-----------------------------------------------------------------------------------+

  @section DESCRIPTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  This file contains the implementation of a mergeable heap data structure, which is a
  type of heap that supports the following operations:
  1. MAKE-HEAP creates a new (empty) heap.
  2. INSERT inserts a new element into the heap.
  3. MINIMUM returns the element with the minimum key in the heap.
  4. EXTRACT-MIN removes and returns the element with the minimum key in the heap.
  5. UNION merges two heaps into a single heap.

  The implementation is based on the Binomial Heap data structure.

  @note The mergeable heap is implemented using C++23.

  @section REVISIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  @section TODO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  @copyright All rights reserved (c) Yehonatan Simian 2024 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

#include <vector>
#include <algorithm>
#include <optional>
#include <cmath>

template <typename T>
class MergeableHeap
{
private:
  struct Node
  {
    T key;
    int degree;
    Node *sibling;
    Node *child;
    constexpr Node(T key) : key(std::move(key)), degree(0), sibling(nullptr), child(nullptr) {}
    constexpr ~Node()
    {
      delete sibling;
      delete child;
    }
  };

private:
  constexpr MergeableHeap(T key) : min(new Node(std::move(key))), head(min), tail(min), size(1) {}

public:
  constexpr MergeableHeap() : min(nullptr), head(nullptr), tail(nullptr), size(0) {}

  constexpr ~MergeableHeap()
  {
    delete head;
  }

  constexpr MergeableHeap(const MergeableHeap &) = delete;
  constexpr MergeableHeap &operator=(const MergeableHeap &) = delete;
  constexpr MergeableHeap(MergeableHeap &&) = delete;
  constexpr MergeableHeap &operator=(MergeableHeap &&) = delete;

  constexpr void insert(T key)
  {
    Node *temp = new Node(std::move(key));
    ++size;

    if (head != nullptr)
    {
      temp->sibling = head;
    }
    else
    {
      tail = temp;
    }
    head = temp;

    if (min == nullptr || key < min->key)
    {
      min = temp;
    }
  }

  constexpr std::optional<std::reference_wrapper<const T>> minimum()
  {
    if (min == nullptr)
    {
      return std::nullopt;
    }
    return std::cref(min->key);
  }

  constexpr std::optional<T> extract_min()
  {
    Node *min_node = remove_min();
    if (min_node == nullptr)
    {
      return std::nullopt;
    }

    if (min_node->child != nullptr)
    {
      // concatenate the children of the min node to the root list
      Node *child = min_node->child;
      Node *last_child = nullptr;

      while (child)
      {
        last_child = child;
        child = child->sibling;
      }

      if (head != nullptr)
      {
        last_child->sibling = head;
      }
      else
      {
        tail = last_child;
      }

      head = min_node->child;
    }

    consolidate();

    if (min == min_node)
    {
      update_min();
    }

    T key = std::move(min_node->key);
    min_node->child = nullptr;
    min_node->sibling = nullptr;
    delete min_node;
    return key;
  }

  constexpr void merge(MergeableHeap<T> &other)
  {
    if (min == nullptr || (other.min != nullptr && other.min->key < min->key))
    {
      min = other.min;
    }

    if (head == nullptr)
    {
      head = other.head;
      tail = other.tail;
    }
    else if (other.head != nullptr)
    {
      tail->sibling = other.head;
      tail = other.tail;
    }

    size += other.size;

    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;
  }

private:
  Node *min;
  Node *head;
  Node *tail;
  size_t size;

  constexpr void update_min()
  {
    min = head;
    if (min == nullptr)
    {
      return;
    }

    Node *curr = head->sibling;
    while (curr != nullptr)
    {
      if (curr->key < min->key)
      {
        min = curr;
      }
      curr = curr->sibling;
    }
  }

  constexpr Node *remove_min()
  {
    if (head == nullptr)
    {
      return nullptr;
    }

    Node *curr = head;
    Node *prev = curr;
    Node *min_node = head;
    Node *prev_min = nullptr;
    curr = curr->sibling;
    while (curr != nullptr)
    {
      if (curr->key < min_node->key)
      {
        min_node = curr;
        prev_min = prev;
      }
      prev = curr;
      curr = curr->sibling;
    }

    if (min_node == head)
    {
      head = head->sibling;
    }
    else
    {
      prev_min->sibling = min_node->sibling;
    }
    if (min_node == tail)
    {
      tail = prev_min;
    }

    --size;
    return min_node;
  }

  constexpr Node *link(Node *tree1, Node *tree2)
  {
    if (tree1->key > tree2->key)
    {
      std::swap(tree1, tree2);
    }
    tree2->sibling = tree1->child;
    tree1->child = tree2;
    tree1->degree += 1;

    return tree1;
  }

  constexpr std::vector<std::vector<Node *>> count_sort()
  {
    size_t max_degree = std::ceil(std::log2(size)) + 1;
    std::vector<std::vector<Node *>> count(max_degree);

    Node *curr = head;
    while (curr != nullptr)
    {
      Node *next = curr->sibling;
      curr->sibling = nullptr;
      int degree = curr->degree;
      count[degree].push_back(curr);
      curr = next;
    }

    return count;
  }

  constexpr void consolidate()
  {
    if (head == nullptr)
    {
      return;
    }

    std::vector<std::vector<Node *>> count = count_sort();

    for (size_t i = 0; i < count.size(); ++i)
    {
      auto &bucket = count[i];
      if (bucket.size() <= 1)
      {
        continue;
      }
      while (bucket.size() > 1)
      {
        Node *tree1 = bucket.back();
        bucket.pop_back();
        Node *tree2 = bucket.back();
        bucket.pop_back();
        Node *new_tree = link(tree1, tree2);
        count[i + 1].push_back(new_tree);
      }
    }

    head = nullptr;
    tail = nullptr;
    for (size_t i = 0; i < count.size(); ++i)
    {
      if (!count[i].empty())
      {
        Node *tree = count[i].front();
        if (head == nullptr)
        {
          head = tree;
        }
        else
        {
          tail->sibling = tree;
        }
        tail = tree;
      }
    }
  }
};