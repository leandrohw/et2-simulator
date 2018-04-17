#include "simulator/heap.h"
#include "gtest/gtest.h"


TEST(HeapTest, Constructor){
  et_simulator::HeapObject heapObj(10);
  ASSERT_EQ(heapObj.getId(), 10);
  ASSERT_TRUE(heapObj.isLive());
  ASSERT_EQ(heapObj.getAllocTime(), -1);
  ASSERT_EQ(heapObj.getType(), "UNKNOWN");
  ASSERT_EQ(heapObj.getAllocCC(), nullptr);
  ASSERT_EQ(heapObj.getDeathTime(), -1);
  ASSERT_EQ(heapObj.getDeathCC(), nullptr);
  ASSERT_EQ(heapObj.getParent(), nullptr);
  ASSERT_EQ(heapObj.getRank(), 0);
  ASSERT_TRUE(heapObj.isRoot());
  ASSERT_EQ(heapObj.getSize(), 1);
  ASSERT_EQ(heapObj.getNumDead(), 0);
}

TEST(HeapTest, SetAlloc){
  et_simulator::HeapObject heapObj(10);
  heapObj.setAlloc(3, 16, "type");
  ASSERT_EQ(heapObj.getAllocTime(), 3);
  ASSERT_EQ(heapObj.getType(), "type");
}

TEST(HeapTest, SetDead){
  et_simulator::HeapObject heapObj(10);
  heapObj.setDead(3);
  ASSERT_FALSE(heapObj.isLive());
  ASSERT_EQ(heapObj.getDeathTime(), 3);
}

TEST(HeapTest, SetAllocCC){
  et_simulator::HeapObject heapObj(10);
  et_simulator::CCNode * ccnode = new et_simulator::CCNode(1, 2, 3, 0);
  heapObj.setAllocCC(ccnode);
  ASSERT_EQ(heapObj.getAllocCC(), ccnode);
}

TEST(HeapTest, SetDeathCC){
  et_simulator::HeapObject heapObj(10);
  et_simulator::CCNode * ccnode = new et_simulator::CCNode(1, 2, 3, 0);
  heapObj.setDeathCC(ccnode);
  ASSERT_EQ(heapObj.getDeathCC(), ccnode);
}

TEST(HeapTest, SetIncrRank){
  et_simulator::HeapObject heapObj(10);
  heapObj.incRank();
  ASSERT_EQ(heapObj.getRank(), 1);
}

TEST(HeapTest, SetParent){
  et_simulator::HeapObject heapObj(10);
  et_simulator::HeapObject * parent = new et_simulator::HeapObject(0);
  heapObj.setParent(parent);
  ASSERT_EQ(heapObj.getParent(), parent);
}

TEST(HeapTest, SetNumDead){
  et_simulator::HeapObject heapObj(10);
  heapObj.setNumDead(10);
  ASSERT_EQ(heapObj.getNumDead(), 10);
}


TEST(HeapTest, incNumDead){
  et_simulator::HeapObject heapObj(10);
  heapObj.incNumDead();
  ASSERT_EQ(heapObj.getNumDead(), 1);
}
