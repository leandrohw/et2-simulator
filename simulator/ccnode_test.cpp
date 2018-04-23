#include "simulator/ccnode.h"
#include "gtest/gtest.h"


TEST(CCNodeTest, Constructor) {
  et_simulator::CCNode node(1, 2, 3, 0);
  EXPECT_EQ(node.get_method_id(), 1);
  EXPECT_EQ(node.get_thread_id(), 2);
  EXPECT_EQ(node.get_first_call(), 3);
  EXPECT_EQ(node.get_last_call(), 3);
  EXPECT_EQ(node.get_parent(), nullptr);
  EXPECT_EQ(node.get_allocated_bytes(), 0);
  EXPECT_EQ(node.get_allocated_objects(), 0);
  EXPECT_EQ(node.get_dead_bytes(), 0);
  EXPECT_EQ(node.get_dead_objects(), 0);
  EXPECT_EQ(node.get_total_allocated_bytes(), 0);
  EXPECT_EQ(node.get_total_dead_bytes(), 0);
  EXPECT_EQ(node.get_allocated_rank(), 0);
}

TEST(CCNodeTest, DemandingNewChild) {
  et_simulator::CCNode  node(0, 0, 0, 0);
  et_simulator::CCNode * child = node.DemandChild(1, 0, 0);
  EXPECT_EQ(node.get_children().size(), 1);
  EXPECT_EQ(node.get_children().at(0), child);
}

TEST(CCNodeTest, DemandingOldChild) {
  et_simulator::CCNode node(0, 0, 0, 0);
  et_simulator::CCNode * old_child = node.DemandChild(1, 0, 0);
  et_simulator::CCNode * new_child = node.DemandChild(1, 0, 0);
  EXPECT_EQ(node.get_children().size(), 1);
  EXPECT_EQ(old_child, new_child);
}

TEST(CCNodeTest, DemandingMultipleChildren) {
  et_simulator::CCNode node(0, 0, 0, 0);
  et_simulator::CCNode * old_child = node.DemandChild(1, 0, 0);
  et_simulator::CCNode * new_child = node.DemandChild(2, 0, 0);
  EXPECT_EQ(node.get_children().size(), 2);
  EXPECT_NE(old_child, new_child);
}

TEST(CCNodeTest, DemandingChildTime) {
  et_simulator::CCNode node(0, 0, 0, 0);
  et_simulator::CCNode * child = node.DemandChild(1, 0, 0);
  EXPECT_EQ(child->get_first_call(), 0);
  EXPECT_EQ(child->get_last_call(), 0);
  child = node.DemandChild(1, 0, 10);
  EXPECT_EQ(child->get_first_call(), 0);
  EXPECT_EQ(child->get_last_call(), 10);
}

TEST(CCNodeTest, DemandingChildBytes) {
  et_simulator::CCNode node(0, 0, 0, 0);
  node.IncrementAllocatedBytes(32);
  et_simulator::CCNode * child = node.DemandChild(1, 0, 0);
  child->IncrementAllocatedBytes(16);
  node.ComputeTotals();
  EXPECT_EQ(node.get_allocated_bytes(), 32);
  EXPECT_EQ(node.get_total_allocated_bytes(), 48);
}
