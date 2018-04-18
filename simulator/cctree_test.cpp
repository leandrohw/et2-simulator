#include "simulator/cctree.h"
#include "simulator/heap.h"
#include "gtest/gtest.h"


TEST(CCTreeTest, Constructor) {
  et_simulator::CCTree cctree;
  EXPECT_EQ(cctree.getDepth(), 0);
  EXPECT_EQ(cctree.getThreadNumber(), 0);
}

TEST(CCTreeTest, Method_Entry) {
  et_simulator::CCTree cctree;
  cctree.handle_method_entry(10, 10, 10);
  EXPECT_EQ(cctree.getDepth(), 1);
  EXPECT_EQ(cctree.getTime(), 1);
  cctree.handle_method_entry(10, 10, 10);
  EXPECT_EQ(cctree.getDepth(), 2);
  EXPECT_EQ(cctree.getTime(), 2);
}

TEST(CCTreeTest, Method_Exit) {
  et_simulator::CCTree cctree;
  cctree.handle_method_entry(474, 0, 1000);
  EXPECT_EQ(cctree.getDepth(), 1);
  EXPECT_EQ(cctree.getTime(), 1);
  cctree.handle_method_exit(474, 0, 1000);
  EXPECT_EQ(cctree.getDepth(), 0);
  EXPECT_EQ(cctree.getTime(), 2);
}

TEST(CCTreeTest, Object_Allocation) {
  et_simulator::CCTree cctree;
  cctree.handle_method_entry(474, 0, 1000);
  cctree.handle_object_allocation(21, 16, "type", 0, 474);
  et_simulator::HeapMap::iterator f =
      et_simulator::HeapObject::theHeap.find(21);
  EXPECT_NE(f, et_simulator::HeapObject::theHeap.end());
  et_simulator::HeapObject* heapObj = et_simulator::HeapObject::theHeap[21];
  EXPECT_EQ(heapObj->getAllocTime(), 1);
}

TEST(CCTreeTest, Object_Death) {
  et_simulator::CCTree cctree;
  cctree.handle_method_entry(474, 0, 1000);
  cctree.handle_object_allocation(21, 16, "type", 0, 474);
  cctree.handle_method_entry(475, 0, 1000);
  cctree.handle_object_death(21);
  et_simulator::HeapMap::iterator f =
      et_simulator::HeapObject::theHeap.find(21);
  EXPECT_NE(f, et_simulator::HeapObject::theHeap.end());
  et_simulator::HeapObject* heapObj = et_simulator::HeapObject::theHeap[21];
  EXPECT_EQ(heapObj->getDeathTime(), 2);
}
