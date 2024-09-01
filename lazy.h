#ifndef LAZY_BINOMIAL_HEAP_H
#define LAZY_BINOMIAL_HEAP_H

#include "mergeable_heap.h"

#include <vector>
#include <algorithm>
#include <optional>
#include <cmath>
#include <queue>
#include <memory>
#include <utility>

/**
 * @class LazyBinomialHeap
 *
 * @brief A fast mergeable heap data structure implementation.
 *
 * @details This mergeable heap is implemented using the binomial heap data structure.
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
class LazyBinomialHeap : public MergeableHeap<T>
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
    T key;                         ///< The key stored in the node.
    int degree;                    ///< The degree of the binomial tree represented by this node.
    std::unique_ptr<Node> sibling; ///< A pointer to the node's sibling.
    std::unique_ptr<Node> child;   ///< A pointer to the node's first child.

    /**
     * @brief Constructs a new node with the given key.
     *
     * This constructor initializes a binomial tree of degree 0 with the given key.
     *
     * @param key The key to store in the node. This key is moved into the node.
     */
    constexpr Node(T key) noexcept(std::is_nothrow_move_constructible_v<T>) : key(std::move(key)), degree(0), sibling(nullptr), child(nullptr) {}

    /**
     * @brief Destroys the node.
     *
     * This destructor deletes the node's sibling and child, if they exist. This will
     * recursively delete all nodes in the sibling and child subtrees.
     */
    constexpr ~Node() = default;
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
  constexpr LazyBinomialHeap(T key) : head(std::make_unique<Node>(std::move(key))), tail(head.get()), min(head.get()), size(1) {}

public:
  /**
   * @brief Constructs a new empty heap.
   *
   * This constructor initializes the heap to be empty. The head, tail, and minimum node
   * pointers are all set to `nullptr`, and the size is set to 0.
   *
   * The time complexity of this operation is O(1).
   */
  constexpr LazyBinomialHeap() noexcept : head(nullptr), tail(nullptr), min(nullptr), size(0) {}

  /**
   * @brief Destroys the heap.
   *
   * This destructor deletes the head node of the heap, which recursively deletes all
   * nodes in the heap.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the heap.
   */
  constexpr ~LazyBinomialHeap() = default;

  /**
   * @brief Inserts a key into the heap.
   *
   * This method inserts a new key into the heap and updates the minimum accordingly.
   *
   * The time complexity of this operation is O(1), because it only involves creating
   * a new node and updating the head and minimum pointers. The heap is not consolidated
   * after each insertion, so the heap may become unbalanced. This is fine because the
   * heap is consolidated lazily only when needed, during the extract_min operation.
   *
   * @param key The key to insert. This key is moved into the heap.
   */
  constexpr void insert(T key) override
  {
    std::unique_ptr<Node> node = std::make_unique<Node>(std::move(key));

    if (++size == 1) // the heap was empty
    {
      head = std::move(node);
      tail = min = head.get();
      return;
    }

    tail->sibling = std::move(node); // concatenate node to the end of the root list
    tail = tail->sibling.get();      // update the tail pointer

    if (key < min->key) // update the minimum if needed
    {
      min = tail;
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
  constexpr std::optional<std::reference_wrapper<const T>> minimum() const noexcept override
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
  constexpr std::optional<T> extract_min() override
  {
    auto min_node = remove_min(); // O(log n)
    if (min_node == nullptr)
    {
      return std::nullopt;
    }

    if (min_node->child != nullptr)
    {
      // concatenate the children of the min node to the end of the root list
      Node *curr = min_node->child.get();
      Node *last_child = nullptr;

      while (curr)
      {
        last_child = curr;
        curr = curr->sibling.get();
      }

      if (head == nullptr)
      {
        head = std::move(min_node->child);
      }
      else
      {
        tail->sibling = std::move(min_node->child);
      }
      tail = last_child;
    }

    consolidate(); // O(log n) amortized

    update_min(); // O(log n)

    return std::move(min_node->key);
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
  constexpr void merge(MergeableHeap<T> &other) override
  {
    LazyBinomialHeap<T> &other_heap = static_cast<LazyBinomialHeap<T> &>(other);

    if (min == nullptr || (other_heap.min != nullptr && other_heap.min->key < min->key))
    {
      min = other_heap.min;
    }

    if (head == nullptr)
    {
      head = std::move(other_heap.head);
    }
    else if (other_heap.head != nullptr)
    {
      tail->sibling = std::move(other_heap.head);
    }
    tail = other_heap.tail;

    size += other_heap.size;

    other_heap.tail = nullptr;
    other_heap.size = 0;
  }

  /**
   * @brief Prints the heap.
   *
   * This method prints the keys of the nodes in the heap using a breadth-first traversal.
   *
   * The time complexity of this operation is O(n), where n is the number of nodes in the heap.
   */
  void print() const
  {
    if (head == nullptr)
    {
      std::cout << "empty.";
      return;
    }

    std::queue<Node *> q;
    if (head != nullptr)
    {
      q.push(head.get());
    }

    while (!q.empty())
    {
      Node *curr = q.front();
      q.pop();
      std::cout << curr->key << ", ";
      if (curr->child != nullptr)
      {
        Node *child = curr->child.get();
        q.push(child);
        while (child->sibling != nullptr)
        {
          child = child->sibling.get();
          q.push(child);
        }
      }
    }

    std::cout << "\b\b.";
  }

  /**
   * @brief Sorts the heap in ascending order.
   *
   * This method extracts the minimum key from the heap until the heap is empty.
   * The extracted keys are then inserted into a temporary heap, which is
   * then merged back into the original heap.
   *
   * The time complexity of this operation is O(n log n), where n is the number of nodes
   * in the heap. This is because the heap is sorted by extracting the minimum key n times,
   * each of which takes O(log n) time, resulting in a total time complexity of O(n log n).
   */
  void sort() override
  {
    this->MergeableHeap<T>::sort(LazyBinomialHeap<T>{});
  }

private:
  std::unique_ptr<Node> head; ///< A pointer to the first node in the root list of the heap.
  Node *tail;                 ///< A pointer to the last node in the root list of the heap.
  Node *min;                  ///< A pointer to the node with the minimum key in the heap.
  size_t size;                ///< The number of nodes in the heap.

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
  constexpr void update_min() noexcept
  {
    min = head.get();
    if (min == nullptr)
    {
      return;
    }

    Node *curr = head->sibling.get();
    while (curr != nullptr)
    {
      if (curr->key < min->key)
      {
        min = curr;
      }
      curr = curr->sibling.get();
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
  constexpr std::unique_ptr<Node> remove_min()
  {
    if (head == nullptr)
    {
      return nullptr;
    }

    Node *curr = head.get();
    Node *prev = curr;
    Node *min_node = head.get();
    Node *prev_min = nullptr;
    curr = curr->sibling.get();
    while (curr != nullptr) // find the minimum node
    {
      if (curr->key < min_node->key)
      {
        min_node = curr;
        prev_min = prev;
      }
      prev = curr;
      curr = curr->sibling.get();
    }

    auto min_node_owner = [&]() -> std::unique_ptr<Node>
    {
      if (min_node == head.get())
      {
        return std::exchange(head, std::move(head->sibling));
      }
      return std::exchange(prev_min->sibling, std::move(min_node->sibling));
    }();

    if (min_node == tail)
    {
      tail = prev_min;
    }

    --size;

    return min_node_owner;
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
  constexpr std::unique_ptr<Node> link(std::unique_ptr<Node> tree1, std::unique_ptr<Node> tree2) const
  {
    if (tree1->key > tree2->key)
    {
      std::swap(tree1, tree2);
    }
    tree2->sibling = std::move(tree1->child);
    tree1->child = std::move(tree2);
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
  constexpr auto count_sort()
  {
    size_t max_degree = std::ceil(std::log2(size)) + 1; // O(1)
    std::vector<std::vector<std::unique_ptr<Node>>> count(max_degree);

    for (std::unique_ptr<Node> curr = std::move(head); curr != nullptr;)
    {
      std::unique_ptr<Node> next = std::move(curr->sibling);
      int degree = curr->degree;
      count[degree].push_back(std::exchange(curr, std::move(next)));
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

    auto count = count_sort(); // O(log n)

    for (size_t i = 0; i < count.size(); ++i)
    {
      auto &bucket = count[i];
      if (bucket.size() <= 1)
      {
        continue;
      }
      while (bucket.size() > 1) // link trees with the same degree
      {
        auto tree1 = std::move(bucket.back());
        bucket.pop_back();
        auto tree2 = std::move(bucket.back());
        bucket.pop_back();
        count[i + 1].push_back(link(std::move(tree1), std::move(tree2)));
      }
    }

    head = nullptr;
    tail = nullptr;
    for (size_t i = 0; i < count.size(); ++i) // concatenate the trees back to the root list
    {
      if (!count[i].empty())
      {
        if (head == nullptr)
        {
          head = std::move(count[i].front());
          tail = head.get();
        }
        else
        {
          tail->sibling = std::move(count[i].front());
          tail = tail->sibling.get();
        }
      }
    }
  }
}; // class LazyBinomialHeap

#endif // LAZY_BINOMIAL_HEAP_H