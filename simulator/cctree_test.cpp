#include "simulator/cctree.h"
#include "simulator/heap.h"
#include "gtest/gtest.h"


TEST(CCTreeTest, Constructor) {
  et_simulator::CCTree cctree;
  ASSERT_EQ(cctree.getDepth(), 0);
  ASSERT_EQ(cctree.getThreadNumber(), 0);

}

TEST(CCTreeTest, Method_Entry) {
  et_simulator::CCTree cctree;
  cctree.handle_method_entry(10, 10, 10);
  ASSERT_EQ(cctree.getDepth(), 1);
  ASSERT_EQ(cctree.getTime(), 1);
  cctree.handle_method_entry(10, 10, 10);
  ASSERT_EQ(cctree.getDepth(), 2);
  ASSERT_EQ(cctree.getTime(), 2);
}

TEST(CCTreeTest, Method_Exit) {
  et_simulator::CCTree cctree;
  cctree.handle_method_entry(474, 0, 1000);
  ASSERT_EQ(cctree.getDepth(), 1);
  ASSERT_EQ(cctree.getTime(), 1);
  cctree.handle_method_exit(474, 0, 1000);
  ASSERT_EQ(cctree.getDepth(), 0);
  ASSERT_EQ(cctree.getTime(), 2);
}

TEST(CCTreeTest, Object_Allocation) {
  et_simulator::CCTree cctree;
  cctree.handle_method_entry(474, 0, 1000);
  et_simulator::HeapObject * heapObj = cctree.handle_object_allocation(21, 16, "type", 0, 474);
  ASSERT_EQ(heapObj->getAllocTime(), 1);
}

TEST(CCTreeTest, Object_Death) {
  et_simulator::CCTree cctree;
  cctree.handle_method_entry(474, 0, 1000);
  et_simulator::HeapObject * heapObj = cctree.handle_object_allocation(21, 16, "type", 0, 474);
  cctree.handle_method_entry(475, 0, 1000);
  cctree.handle_object_death(21);
  ASSERT_EQ(heapObj->getDeathTime(), 2);
}
