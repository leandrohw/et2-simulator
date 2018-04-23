#include "simulator/heap.h"
#include "gtest/gtest.h"


TEST(HeapTest, Constructor){
  et_simulator::HeapObject heapObj(10);
  EXPECT_EQ(heapObj.getId(), 10);
  EXPECT_TRUE(heapObj.isLive());
  EXPECT_EQ(heapObj.getAllocTime(), -1);
  EXPECT_EQ(heapObj.getType(), "UNKNOWN");
  EXPECT_EQ(heapObj.getAllocCC(), nullptr);
  EXPECT_EQ(heapObj.getDeathTime(), -1);
  EXPECT_EQ(heapObj.getDeathCC(), nullptr);
  EXPECT_EQ(heapObj.getParent(), nullptr);
  EXPECT_EQ(heapObj.getRank(), 0);
  EXPECT_TRUE(heapObj.isRoot());
  EXPECT_EQ(heapObj.get_allocation_bytes(), -1);
  EXPECT_EQ(heapObj.getNumDead(), 0);
}

TEST(HeapTest, SetAlloc){
  et_simulator::HeapObject heapObj(10);
  heapObj.setAlloc(3, 16, "type");
  EXPECT_EQ(heapObj.getAllocTime(), 3);
  EXPECT_EQ(heapObj.getType(), "type");
}

TEST(HeapTest, SetDead){
  et_simulator::HeapObject heapObj(10);
  heapObj.setDead(3);
  EXPECT_FALSE(heapObj.isLive());
  EXPECT_EQ(heapObj.getDeathTime(), 3);
}

TEST(HeapTest, SetAllocCC){
  et_simulator::HeapObject heapObj(10);
  et_simulator::CCNode ccnode(1, 2, 3, 0);
  heapObj.setAllocCC(&ccnode);
  EXPECT_EQ(heapObj.getAllocCC(), &ccnode);
}

TEST(HeapTest, SetDeathCC){
  et_simulator::HeapObject heapObj(10);
  et_simulator::CCNode ccnode(1, 2, 3, 0);
  heapObj.setDeathCC(&ccnode);
  EXPECT_EQ(heapObj.getDeathCC(), &ccnode);
}

TEST(HeapTest, SetIncrRank){
  et_simulator::HeapObject heapObj(10);
  heapObj.incRank();
  EXPECT_EQ(heapObj.getRank(), 1);
}

TEST(HeapTest, SetParent){
  et_simulator::HeapObject heapObj(10);
  et_simulator::HeapObject parent(0);
  heapObj.setParent(&parent);
  EXPECT_EQ(heapObj.getParent(), &parent);
}

TEST(HeapTest, SetNumDead){
  et_simulator::HeapObject heapObj(10);
  heapObj.setNumDead(10);
  EXPECT_EQ(heapObj.getNumDead(), 10);
}


TEST(HeapTest, incNumDead){
  et_simulator::HeapObject heapObj(10);
  heapObj.incNumDead();
  EXPECT_EQ(heapObj.getNumDead(), 1);
}
