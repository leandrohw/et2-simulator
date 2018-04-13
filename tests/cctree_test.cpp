#include "simulator/cctree.h"
#include "gtest/gtest.h"


TEST(CCTreeTest, Constructor) {
  CCTree cctree;
  ASSERT_EQ(cctree.getDepth(), 0);
  ASSERT_EQ(cctree.getThreadNumber(), 0);

}

TEST(CCTreeTest, Method_Entry) {
  CCTree cctree;
  cctree.handle_method_entry(10, 10, 10);
  ASSERT_EQ(cctree.getDepth(), 1);
  ASSERT_EQ(cctree.getTime(), 1);
  cctree.handle_method_entry(10, 10, 10);
  ASSERT_EQ(cctree.getDepth(), 2);
  ASSERT_EQ(cctree.getTime(), 2);
}

TEST(CCTreeTest, Method_Exit) {
  CCTree cctree;
  cctree.handle_method_entry(474, 0, 1000);
  ASSERT_EQ(cctree.getDepth(), 1);
  ASSERT_EQ(cctree.getTime(), 1);
  cctree.handle_method_exit(474, 0, 1000);
  ASSERT_EQ(cctree.getDepth(), 0);
  ASSERT_EQ(cctree.getTime(), 2);
}
