#include "simulator/simulator.h"
#include "gtest/gtest.h"

TEST(SimulatorTest, GetGreet) {
  EXPECT_EQ("Hello Bazel", "Hello Bazel");
}
