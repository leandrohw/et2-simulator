#include "simulator/heap.h"
#include "gtest/gtest.h"


TEST(HeapTest, Constructor){
  et_simulator::HeapObject heapObj(10);
  EXPECT_EQ(heapObj.get_id(), 10);
  EXPECT_TRUE(heapObj.is_live());
  EXPECT_EQ(heapObj.get_allocation_time(), -1);
  EXPECT_EQ(heapObj.get_type(), "UNKNOWN");
  EXPECT_EQ(heapObj.get_allocation_context(), nullptr);
  EXPECT_EQ(heapObj.get_death_time(), -1);
  EXPECT_EQ(heapObj.get_death_context(), nullptr);
  EXPECT_EQ(heapObj.get_parent(), nullptr);
  EXPECT_EQ(heapObj.get_rank(), 0);
  EXPECT_TRUE(heapObj.is_root());
  EXPECT_EQ(heapObj.get_allocation_bytes(), -1);
  EXPECT_EQ(heapObj.get_number_of_dead_objects(), 0);
}

TEST(HeapTest, SetAlloc){
  et_simulator::HeapObject heapObj(10);
  heapObj.MarkAllocated(3, 16, "type");
  EXPECT_EQ(heapObj.get_allocation_time(), 3);
  EXPECT_EQ(heapObj.get_type(), "type");
}

TEST(HeapTest, SetDead){
  et_simulator::HeapObject heapObj(10);
  heapObj.MarkDead(3);
  EXPECT_FALSE(heapObj.is_live());
  EXPECT_EQ(heapObj.get_death_time(), 3);
}

TEST(HeapTest, SetAllocCC){
  et_simulator::HeapObject heapObj(10);
  et_simulator::CCNode ccnode(1, 2, 3, 0);
  heapObj.MarkAllocatedCC(&ccnode);
  EXPECT_EQ(heapObj.get_allocation_context(), &ccnode);
}

TEST(HeapTest, SetDeathCC){
  et_simulator::HeapObject heapObj(10);
  et_simulator::CCNode ccnode(1, 2, 3, 0);
  heapObj.setDeathCC(&ccnode);
  EXPECT_EQ(heapObj.get_death_context(), &ccnode);
}

TEST(HeapTest, SetIncrRank){
  et_simulator::HeapObject heapObj(10);
  heapObj.IncrementRank();
  EXPECT_EQ(heapObj.get_rank(), 1);
}

TEST(HeapTest, SetParent){
  et_simulator::HeapObject heapObj(10);
  et_simulator::HeapObject parent_(0);
  heapObj.set_parent(&parent);
  EXPECT_EQ(heapObj.get_parent(), &parent);
}

TEST(HeapTest, SetNumDead){
  et_simulator::HeapObject heapObj(10);
  heapObj.set_number_of_dead_objects(10);
  EXPECT_EQ(heapObj.get_number_of_dead_objects(), 10);
}


TEST(HeapTest, IncrementNumberOfDeadObjects){
  et_simulator::HeapObject heapObj(10);
  heapObj.IncrementNumberOfDeadObjects();
  EXPECT_EQ(heapObj.get_number_of_dead_objects(), 1);
}
