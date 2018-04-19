#include "simulator/simulator.h"
#include "gtest/gtest.h"

TEST(Simulator, Constructor) {
  et_simulator::Simulator simulator("", "");
  EXPECT_EQ(simulator.getNamesFile(), "");
  EXPECT_EQ(simulator.getTraceFile(), "");
}

TEST(Simulator, Object_Allocation_Event) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.execute("A 1 2 type 0"));
  EXPECT_FALSE(simulator.execute("A 1 2 type"));
  EXPECT_FALSE(simulator.execute("A 1 2 type 4 5"));
}

TEST(Simulator, Object_Object_Update) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.execute("U 1 2 3 0"));
  EXPECT_FALSE(simulator.execute("U 1 2 3"));
  EXPECT_FALSE(simulator.execute("U 1 2"));
  EXPECT_FALSE(simulator.execute("U 1 2 3 4 5"));
}

TEST(Simulator, Object_Method_Entry) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.execute("M 1 2 3"));
  EXPECT_FALSE(simulator.execute("M 1 2"));
  EXPECT_FALSE(simulator.execute("M 1 2 3 4"));
}
