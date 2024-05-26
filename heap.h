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
    std::vector<Node *> children;
    Node(T key) : key(key), degree(0) {}
    ~Node()
    {
      std::for_each(children.begin(), children.end(), [](Node *child)
                    { delete child; });
      children.clear();
    }
  };

public:
  MergeableHeap() : min(nullptr) {}

  MergeableHeap(T key) : min(new Node(key))
  {
    roots.push_back(min);
  }

  ~MergeableHeap()
  {
    std::for_each(roots.begin(), roots.end(), [](Node *root)
                  { delete root; });
    roots.clear();
    min = nullptr;
  }

  MergeableHeap(const MergeableHeap &) = delete;
  MergeableHeap &operator=(const MergeableHeap &) = delete;
  MergeableHeap(MergeableHeap &&) = delete;
  MergeableHeap &operator=(MergeableHeap &&) = delete;

  void insert(T key)
  {
    MergeableHeap temp(key);
    merge(temp);
  }

  std::optional<T> minimum()
  {
    if (min == nullptr)
    {
      return std::nullopt;
    }
    return min->key;
  }

  std::optional<T> extract_min()
  {
    Node *min_node = min;
    if (min_node == nullptr)
    {
      return std::nullopt;
    }
    roots.erase(std::remove(roots.begin(), roots.end(), min_node), roots.end());
    MergeableHeap temp{};
    temp.roots = min_node->children;
    merge(temp);
    update_min();
    return min_node->key;
  }

  void merge(MergeableHeap<T> &other)
  {
    roots.insert(roots.end(), other.roots.begin(), other.roots.end());
    update_min();
    other.roots.clear();
  }

private:
  Node *min;
  std::vector<Node *> roots;

  void update_min()
  {
    min = nullptr;
    if (!roots.empty())
    {
      min = *std::min_element(roots.begin(), roots.end(),
                              [](const Node *a, const Node *b)
                              { return a->key < b->key; });
    }
  }

  void link(Node *tree1, Node *tree2)
  {
    if (tree1->value > tree2->value)
    {
      swap(tree1, tree2);
    }
    tree1->children.push_back(tree2);
    tree1->degree += 1;
  }

  void consolidate()
  {
    int max_degree = static_cast<int>(std::floor(std::log2(roots.size()))) + 1;
    std::vector<Node *> degree_to_tree(max_degree + 1, nullptr);

    while (!roots.empty())
    {
      Node *current = roots[0];
      roots.erase(roots.begin());
      int degree = current->degree;
      while (degree_to_tree[degree] != nullptr)
      {
        Node *other = degree_to_tree[degree];
        degree_to_tree[degree] = nullptr;
        if (current->value < other->value)
        {
          link(current, other);
        }
        else
        {
          link(other, current);
          current = other;
        }
        ++degree;
      }
      degree_to_tree[degree] = current;
    }

    min = nullptr;
    roots.clear();
    for (Node *tree : degree_to_tree)
    {
      if (tree != nullptr)
      {
        roots.push_back(tree);
        if (min == nullptr || tree->value < min->value)
        {
          min = tree;
        }
      }
    }
  }
};