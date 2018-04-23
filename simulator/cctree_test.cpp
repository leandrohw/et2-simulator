#include "simulator/cctree.h"
#include "simulator/heap.h"
#include "gtest/gtest.h"


TEST(CCTreeTest, Constructor) {
  et_simulator::CCTree cctree;
  EXPECT_EQ(cctree.get_depth(), 0);
  EXPECT_EQ(cctree.get_thread_number(), 0);
}

TEST(CCTreeTest, Method_Entry) {
  et_simulator::CCTree cctree;
  cctree.HandleMethodEntry(10, 10, 10);
  EXPECT_EQ(cctree.get_depth(), 1);
  EXPECT_EQ(cctree.get_time(), 1);
  cctree.HandleMethodEntry(10, 10, 10);
  EXPECT_EQ(cctree.get_depth(), 2);
  EXPECT_EQ(cctree.get_time(), 2);
}

TEST(CCTreeTest, Method_Exit) {
  et_simulator::CCTree cctree;
  cctree.HandleMethodEntry(474, 0, 1000);
  EXPECT_EQ(cctree.get_depth(), 1);
  EXPECT_EQ(cctree.get_time(), 1);
  cctree.HandleMethodExit(474, 0, 1000);
  EXPECT_EQ(cctree.get_depth(), 0);
  EXPECT_EQ(cctree.get_time(), 2);
}

TEST(CCTreeTest, Object_Allocation) {
  et_simulator::CCTree cctree;
  cctree.HandleMethodEntry(474, 0, 1000);
  cctree.HandleObjectAllocation(21, 16, "type", 0);
  EXPECT_NE(et_simulator::HeapObject::theHeap.find(21),
            et_simulator::HeapObject::theHeap.end());
  et_simulator::HeapObject* heapObj = et_simulator::HeapObject::theHeap[21];
  EXPECT_EQ(heapObj->getAllocTime(), 1);
}

TEST(CCTreeTest, Object_Death) {
  et_simulator::CCTree cctree;
  cctree.HandleMethodEntry(474, 0, 1000);
  cctree.HandleObjectAllocation(21, 16, "type", 0);
  cctree.HandleMethodEntry(475, 0, 1000);
  cctree.HandleObjectDeath(21);
  EXPECT_NE(et_simulator::HeapObject::theHeap.find(21),
            et_simulator::HeapObject::theHeap.end());
  et_simulator::HeapObject* heapObj = et_simulator::HeapObject::theHeap[21];
  EXPECT_EQ(heapObj->getDeathTime(), 2);
}
