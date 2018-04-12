#include "simulator/ccnode.h"
#include "gtest/gtest.h"

TEST(DemandTest, DemandingNewChild) {
  CCNode * node = new CCNode(0, 0, 0, 0);
  CCNode * child = node->demand_child(1, 0, 0);
  ASSERT_EQ(node->getChildren().size(), 1);
  ASSERT_EQ(node->getChildren().at(0), child);

}

TEST(ChildrenTest, DemandingOldChild) {
  CCNode * node = new CCNode(0, 0, 0, 0);
  CCNode * old_child = node->demand_child(1, 0, 0);
  CCNode * new_child = node->demand_child(1, 0, 0);
  ASSERT_EQ(node->getChildren().size(), 1);
  ASSERT_EQ(old_child, new_child);

}

TEST(ChildrenTest, DemandingMultipleChildren) {
  CCNode * node = new CCNode(0, 0, 0, 0);
  CCNode * old_child = node->demand_child(1, 0, 0);
  CCNode * new_child = node->demand_child(2, 0, 0);
  ASSERT_EQ(node->getChildren().size(), 2);
  ASSERT_NE(old_child, new_child);

}
