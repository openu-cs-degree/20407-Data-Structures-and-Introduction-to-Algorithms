#include <gtest/gtest.h>
#include "heap.h"

// g++ -o test test.cc -lgtest -lpthread

TEST(MergeableHeapTest, Insert)
{
  MergeableHeap<int> h{};
  h.insert(10);
  h.insert(5);
  h.insert(15);
  EXPECT_EQ(h.minimum(), 5);
}

TEST(MergeableHeapTest, ExtractMin)
{
  MergeableHeap<int> h{};
  h.insert(10);
  h.insert(5);
  h.insert(15);
  EXPECT_EQ(h.extract_min(), 5);
  EXPECT_EQ(h.extract_min(), 10);
  EXPECT_EQ(h.extract_min(), 15);
  EXPECT_EQ(h.extract_min(), std::nullopt);
}

TEST(MergeableHeapTest, Merge)
{
  MergeableHeap<int> h1{};
  h1.insert(10);
  h1.insert(5);
  MergeableHeap<int> h2{};
  h2.insert(15);
  h2.insert(20);
  h1.merge(h2);
  EXPECT_EQ(h1.minimum(), 5);
  EXPECT_EQ(h1.extract_min(), 5);
  EXPECT_EQ(h1.extract_min(), 10);
  EXPECT_EQ(h1.extract_min(), 15);
  EXPECT_EQ(h1.extract_min(), 20);
  EXPECT_EQ(h1.extract_min(), std::nullopt);
}

TEST(MergeableHeapTest, EmptyHeap)
{
  MergeableHeap<int> h{};
  EXPECT_EQ(h.minimum(), std::nullopt);
  EXPECT_EQ(h.extract_min(), std::nullopt);
}

TEST(MergeableHeapTest, LargeHeap)
{
  MergeableHeap<int> h{};
  for (int i = 1000; i > 0; --i)
  {
    h.insert(i);
  }
  EXPECT_EQ(h.minimum(), 1);
  EXPECT_EQ(h.extract_min(), 1);
  EXPECT_EQ(h.extract_min(), 2);
  EXPECT_EQ(h.extract_min(), 3);
  EXPECT_EQ(h.extract_min(), 4);
}

TEST(MergeableHeapTest, MergeEmpty)
{
  MergeableHeap<int> h1{};
  MergeableHeap<int> h2{};
  h1.merge(h2);
  EXPECT_EQ(h1.minimum(), std::nullopt);
  EXPECT_EQ(h1.extract_min(), std::nullopt);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}