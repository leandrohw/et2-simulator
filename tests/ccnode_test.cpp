#include "simulator/ccnode.h"
#include "gtest/gtest.h"

TEST(DefaultTest, Constructor) {
  CCNode * node = new CCNode(1, 2, 3, 0);
  ASSERT_EQ(node->getMethodId(), 1);
  ASSERT_EQ(node->getThreadId(), 2);
  ASSERT_EQ(node->getFirstCall(), 3);
  ASSERT_EQ(node->getLastCall(), 3);
  ASSERT_EQ(node->getParent(), nullptr);
  ASSERT_EQ(node->getAllocBytes(), 0);
  ASSERT_EQ(node->getAllocObjects(), 0);
  ASSERT_EQ(node->getDeadBytes(), 0);
  ASSERT_EQ(node->getDeadObjects(), 0);
  ASSERT_EQ(node->getTotalAllocBytes(), 0);
  ASSERT_EQ(node->getTotalDeadBytes(), 0);
  ASSERT_EQ(node->getAllocRank(), 0);
}

TEST(DemandTest, DemandingNewChild) {
  CCNode * node = new CCNode(0, 0, 0, 0);
  CCNode * child = node->demand_child(1, 0, 0);
  ASSERT_EQ(node->getChildren().size(), 1);
  ASSERT_EQ(node->getChildren().at(0), child);

}

TEST(DemandTest, DemandingOldChild) {
  CCNode * node = new CCNode(0, 0, 0, 0);
  CCNode * old_child = node->demand_child(1, 0, 0);
  CCNode * new_child = node->demand_child(1, 0, 0);
  ASSERT_EQ(node->getChildren().size(), 1);
  ASSERT_EQ(old_child, new_child);

}

TEST(DemandTest, DemandingMultipleChildren) {
  CCNode * node = new CCNode(0, 0, 0, 0);
  CCNode * old_child = node->demand_child(1, 0, 0);
  CCNode * new_child = node->demand_child(2, 0, 0);
  ASSERT_EQ(node->getChildren().size(), 2);
  ASSERT_NE(old_child, new_child);

}

TEST(DemandTest, DemandingChildTime) {
  CCNode * node = new CCNode(0, 0, 0, 0);
  CCNode * child = node->demand_child(1, 0, 0);
  ASSERT_EQ(child->getFirstCall(), 0);
  ASSERT_EQ(child->getLastCall(), 0);

  child = node->demand_child(1, 0, 10);
  ASSERT_EQ(child->getFirstCall(), 0);
  ASSERT_EQ(child->getLastCall(), 10);

}


TEST(DemandComputeTotal, DemandingChildTime) {
  CCNode * node = new CCNode(0, 0, 0, 0);
  node->incAllocBytes(32);

  CCNode * child = node->demand_child(1, 0, 0);
  child->incAllocBytes(16);

  node->computeTotals();
  ASSERT_EQ(node->getAllocBytes(), 32);
  ASSERT_EQ(node->getTotalAllocBytes(), 48);

}
