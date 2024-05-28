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

  The implementation is based on the Binomial Heap data structure:
  https://en.wikipedia.org/wiki/Binomial_heap
  Note that unlike the Binomial Heap in the link above, this implementation does not
  support the DECREASE-KEY operation. In addition, the implementation is optimized such
  that each operation runs in O(1) time complexity except for the EXTRACT-MIN operation
  which runs in an amortized time complexity of O(log n).

  @note The implementation is constexpr-friendly.
  @note The implementation does not handle memory allocation failures for simplicity.

  @section COMPILATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  To compile this project, I have used the following command:

  g++ -std=c++23 -Wall -Wextra -Werror -Wpedantic -o main main.cpp

  @note The mergeable heap is implemented using C++23, and was tested using gcc 13.1.0.

  @copyright All rights reserved (c) Yehonatan Simian 2024 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

#include <vector>
#include <algorithm>
#include <optional>
#include <cmath>

/**
 * @class MergeableHeap
 * @brief A mergeable heap data structure.
 *
 * This class represents a mergeable heap data structure. It provides methods for
 * inserting elements, merging heaps, and finding the minimum element.
 *
 * @details The mergeable heap is implemented using the binomial heap data structure.
 * A binomial heap is a collection of binomial trees, where each binomial tree is a
 * heap-ordered tree. A binomial tree of order k is a tree with 2^k nodes, where each
 * node has a key and a pointer to its parent and its leftmost child. The binomial heap
 * is a collection of binomial trees of different orders, where each order is represented
 * by a linked list of binomial trees. The root list of the heap is a linked list of the
 * roots of the binomial trees in the heap, hence its size is logarithmic in the number of
 * nodes in the heap in the average case. The current implementation of the binomial heap
 * is lazy, meaning that the heap is consolidated only when needed, during the extract_min
 * operation. This allows for O(1) insertions and merges, and an amortized O(log n) extract_min.
 * Amortized analysis is used to analyze the time complexity of the consolidate operation,
 *
 * @tparam T The type of the elements stored in the heap. `T` must be movable, as it is
 * moved into the heap in the insert method.
 */
template <typename T>
class MergeableHeap
{
private:
  /**
   * @struct Node
   * @brief A node in the heap.
   *
   * This struct represents a node in the heap. Each node contains a key of type `T`,
   * a degree representing the number of children, and pointers to its sibling and child nodes.
   *
   * @note The linked-list is implemented using the left-child, right-sibling (LCRS) idiom.
   *
   * @tparam T The type of the key stored in the node. `T` must be movable, as it is moved
   * into the node in the constructor.
   */
  struct Node
  {
    T key;         ///< The key stored in the node.
    int degree;    ///< The degree of the binomial tree represented by this node.
    Node *sibling; ///< A pointer to the node's sibling.
    Node *child;   ///< A pointer to the node's first child.

    /**
     * @brief Constructs a new node with the given key.
     *
     * This constructor initializes a binomial tree of degree 0 with the given key.
     *
     * @param key The key to store in the node. This key is moved into the node.
     */
    constexpr Node(T key) : key(std::move(key)), degree(0), sibling(nullptr), child(nullptr) {}

    /**
     * @brief Destroys the node.
     *
     * This destructor deletes the node's sibling and child, if they exist. This will
     * recursively delete all nodes in the sibling and child subtrees.
     */
    constexpr ~Node()
    {
      delete sibling;
      delete child;
    }
  };

private:
  /**
   * @brief Constructs a new heap with a single key.
   *
   * This constructor initializes the heap with a single key, resulting in
   * a single 0 degree binomial tree in the heap.
   *
   * The time complexity of this operation is O(1).
   *
   * @param key The key to store in the heap. This key is moved into the heap.
   */
  constexpr MergeableHeap(T key) : min(new Node(std::move(key))), head(min), tail(min), size(1) {}

public:
  /**
   * @brief Constructs a new empty heap.
   *
   * This constructor initializes the heap to be empty. The head, tail, and minimum node
   * pointers are all set to `nullptr`, and the size is set to 0.
   *
   * The time complexity of this operation is O(1).
   */
  constexpr MergeableHeap() : min(nullptr), head(nullptr), tail(nullptr), size(0) {}

  /**
   * @brief Destroys the heap.
   *
   * This destructor deletes the head node of the heap, which recursively deletes all
   * nodes in the heap.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the heap.
   */
  constexpr ~MergeableHeap()
  {
    delete head;
  }

  /**
   * @brief Copy constructor (deleted).
   *
   * This constructor is deleted because `MergeableHeap` objects should not be copied.
   * This is because they own their nodes, so copying a `MergeableHeap` would result in
   * multiple `MergeableHeap` objects owning the same nodes, which could lead to
   * double deletion.
   */
  constexpr MergeableHeap(const MergeableHeap &) = delete;

  /**
   * @brief Copy assignment operator (deleted).
   *
   * This operator is deleted because `MergeableHeap` objects should not be copied.
   * This is because they own their nodes, so copying a `MergeableHeap` would result in
   * multiple `MergeableHeap` objects owning the same nodes, which could lead to
   * double deletion.
   */
  constexpr MergeableHeap &operator=(const MergeableHeap &) = delete;

  /**
   * @brief Move constructor (deleted).
   *
   * This constructor is deleted because `MergeableHeap` objects should not be moved.
   * This is because they own their nodes, so moving a `MergeableHeap` would result in
   * the moved-from `MergeableHeap` still owning the nodes, which could lead to
   * double deletion.
   */
  constexpr MergeableHeap(MergeableHeap &&) = delete;

  /**
   * @brief Move assignment operator (deleted).
   *
   * This operator is deleted because `MergeableHeap` objects should not be moved.
   * This is because they own their nodes, so moving a `MergeableHeap` would result in
   * the moved-from `MergeableHeap` still owning the nodes, which could lead to
   * double deletion.
   */
  constexpr MergeableHeap &operator=(MergeableHeap &&) = delete;

  /**
   * @brief Inserts a key into the heap.
   *
   * This method inserts a new key into the heap. The key is moved into a new node,
   * which is inserted at the head of the root list. If the key is smaller than the
   * current minimum, the new node becomes the new minimum.
   *
   * The time complexity of this operation is O(1), because it only involves creating
   * a new node and updating the head and minimum pointers. The heap is not consolidated
   * after each insertion, so the heap may become unbalanced. This is fine because the
   * heap is consolidated lazily only when needed, during the extract_min operation.
   *
   * @param key The key to insert. This key is moved into the heap.
   */
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

  /**
   * @brief Returns the minimum key in the heap.
   *
   * This method returns a reference to the minimum key in the heap, or `std::nullopt`
   * if the heap is empty. The reference is wrapped in a `std::reference_wrapper` to
   * allow it to be used in contexts where a reference cannot be used.
   *
   * The time complexity of this operation is O(1), because the minimum key is stored
   * in the `min` pointer, which is updated whenever a new minimum key is inserted.
   *
   * @return A reference to the minimum key, or `std::nullopt` if the heap is empty.
   */
  constexpr std::optional<std::reference_wrapper<const T>> minimum() const
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
   * This method removes the minimum key from the heap and returns it. If the minimum
   * key has children, they are added to the root list. The heap is then consolidated
   * to maintain the heap property. If the heap is empty, `std::nullopt` is returned.
   *
   * The time complexity of this operation is O(log n) in the average case (amotrized),
   * and O(n) in the worst case where n is the number of nodes in the heap. This is
   * because the heap is consolidated after the minimum key is removed, which involves
   * linking binomial trees of the same degree until there are no two trees with the same degree.
   *
   * @return The minimum key, or `std::nullopt` if the heap is empty.
   */
  constexpr std::optional<T> extract_min()
  {
    Node *min_node = remove_min(); // O(log n)
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

    consolidate(); // O(log n) amortized

    if (min == min_node)
    {
      update_min(); // O(log n)
    }

    T key = std::move(min_node->key);
    min_node->child = nullptr;
    min_node->sibling = nullptr;
    delete min_node;
    return key;
  }

  /**
   * @brief Merges another heap into this heap.
   *
   * This method merges another heap into this heap. The root list of the other heap
   * is concatenated to the root list of this heap, and the size of this heap is
   * increased by the size of the other heap, and the minimum is updated accordingly.
   *
   * @note The other heap is left empty after the merge.
   *
   * The time complexity of this operation is O(1), because it only involves updating
   * the head, tail, minimum and size pointers of this heap. The heap is not consolidated
   * after the merge, so the heap may become unbalanced. This is fine because the heap
   * is consolidated lazily only when needed, during the extract_min operation.
   *
   * @param other The heap to merge into this heap.
   */
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
  Node *min;   ///< A pointer to the node with the minimum key in the heap.
  Node *head;  ///< A pointer to the first node in the root list of the heap.
  Node *tail;  ///< A pointer to the last node in the root list of the heap.
  size_t size; ///< The number of nodes in the heap.

  /**
   * @brief Updates the minimum node pointer.
   *
   * This method updates the `min` pointer to point to the node with the minimum key
   * in the root list. If the heap is empty, `min` is set to `nullptr`.
   *
   * The time complexity of this operation is O(log n), where n is the number of nodes in
   * the heap. This is because the `min` pointer is updated by iterating over the root list,
   * which is logarithmic in the number of nodes in the heap.
   */
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

  /**
   * @brief Removes and returns the minimum node.
   *
   * This method removes the node with the minimum key from the root list and returns
   * it. If the heap is empty, `nullptr` is returned. The `size` is decremented by 1.
   *
   * The time complexity of this operation is O(log n), where n is the number of nodes in
   * the heap. This is because the minimum node is found by iterating over the root list,
   * which is logarithmic in the number of nodes in the heap.
   *
   * @return The minimum node, or `nullptr` if the heap is empty.
   */
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

  /**
   * @brief Links two trees in the heap.
   *
   * This method links two binomial trees, resulting in a binomial tree with a higher degree.
   * The tree with the larger key becomes the child of the tree with the smaller key.
   * The degree of the parent tree is incremented by 1.
   *
   * The time complexity of this operation is O(1), because it only involves updating the
   * sibling and child pointers of the trees.
   *
   * @param tree1 The first tree to link.
   * @param tree2 The second tree to link.
   * @return The resulting tree after the link.
   */
  constexpr Node *link(Node *tree1, Node *tree2) const
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

  /**
   * @brief Performs a count sort on the root list.
   *
   * This method performs a count sort on the root list based on the degree of the
   * nodes. It returns a vector of vectors, where each inner vector contains the nodes
   * with the same degree.
   *
   * The time complexity of this operation is O(log n), where n is the number of nodes
   * in the heap. This is because the nodes are sorted into buckets based on their degree,
   * which is bounded by the logarithm of the number of nodes in the heap.
   *
   * @return A vector of vectors containing the nodes sorted by degree.
   */
  constexpr std::vector<std::vector<Node *>> count_sort()
  {
    size_t max_degree = std::ceil(std::log2(size)) + 1; // O(1)
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

  /**
   * @brief Consolidates the heap.
   *
   * This method consolidates the heap to ensure that no two trees have the same
   * degree. It performs a count sort on the root list, and then links together any
   * trees with the same degree until all trees have different degrees.
   *
   * The time complexity of this operation is O(log n) in the average case (amortized),
   * and O(n) in the worst case where n is the number of nodes in the heap. This is
   * because the heap is consolidated by linking binomial trees of the same degree until
   * there are no two trees with the same degree; this process involves iterating over
   * the root list, which is ideally logarithmic in the number of nodes in the heap,
   * however in the worst case the root list may contain all the nodes in the heap
   * (for example by merging n heaps of size 1), in which case the time complexity
   * becomes linear in the number of nodes in the heap.
   */
  constexpr void consolidate()
  {
    if (head == nullptr)
    {
      return;
    }

    std::vector<std::vector<Node *>> count = count_sort(); // O(log n)

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
}; // class MergeableHeap