#include "simulator/ccnode.h"
#include "gtest/gtest.h"


TEST(CCNodeTest, Constructor) {
  et_simulator::CCNode node(1, 2, 3, 0);
  EXPECT_EQ(node.getMethodId(), 1);
  EXPECT_EQ(node.getThreadId(), 2);
  EXPECT_EQ(node.getFirstCall(), 3);
  EXPECT_EQ(node.getLastCall(), 3);
  EXPECT_EQ(node.getParent(), nullptr);
  EXPECT_EQ(node.getAllocBytes(), 0);
  EXPECT_EQ(node.getAllocObjects(), 0);
  EXPECT_EQ(node.getDeadBytes(), 0);
  EXPECT_EQ(node.getDeadObjects(), 0);
  EXPECT_EQ(node.getTotalAllocBytes(), 0);
  EXPECT_EQ(node.getTotalDeadBytes(), 0);
  EXPECT_EQ(node.getAllocRank(), 0);
}

TEST(CCNodeTest, DemandingNewChild) {
  et_simulator::CCNode  node(0, 0, 0, 0);
  et_simulator::CCNode * child = node.demand_child(1, 0, 0);
  EXPECT_EQ(node.getChildren().size(), 1);
  EXPECT_EQ(node.getChildren().at(0), child);
}

TEST(CCNodeTest, DemandingOldChild) {
  et_simulator::CCNode node(0, 0, 0, 0);
  et_simulator::CCNode * old_child = node.demand_child(1, 0, 0);
  et_simulator::CCNode * new_child = node.demand_child(1, 0, 0);
  EXPECT_EQ(node.getChildren().size(), 1);
  EXPECT_EQ(old_child, new_child);
}

TEST(CCNodeTest, DemandingMultipleChildren) {
  et_simulator::CCNode node(0, 0, 0, 0);
  et_simulator::CCNode * old_child = node.demand_child(1, 0, 0);
  et_simulator::CCNode * new_child = node.demand_child(2, 0, 0);
  EXPECT_EQ(node.getChildren().size(), 2);
  EXPECT_NE(old_child, new_child);
}

TEST(CCNodeTest, DemandingChildTime) {
  et_simulator::CCNode node(0, 0, 0, 0);
  et_simulator::CCNode * child = node.demand_child(1, 0, 0);
  EXPECT_EQ(child->getFirstCall(), 0);
  EXPECT_EQ(child->getLastCall(), 0);
  child = node.demand_child(1, 0, 10);
  EXPECT_EQ(child->getFirstCall(), 0);
  EXPECT_EQ(child->getLastCall(), 10);
}

TEST(CCNodeTest, DemandingChildBytes) {
  et_simulator::CCNode node(0, 0, 0, 0);
  node.incAllocBytes(32);
  et_simulator::CCNode * child = node.demand_child(1, 0, 0);
  child->incAllocBytes(16);
  node.computeTotals();
  EXPECT_EQ(node.getAllocBytes(), 32);
  EXPECT_EQ(node.getTotalAllocBytes(), 48);
}
