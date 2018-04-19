#include "simulator/simulator.h"
#include "gtest/gtest.h"

TEST(SimulatorTest, Constructor) {
  et_simulator::Simulator simulator("", "");
  EXPECT_EQ(simulator.getNamesFile(), "");
  EXPECT_EQ(simulator.getTraceFile(), "");
}

TEST(SimulatorTest, Parse_Object_Allocation_Event) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.execute("A 1 2 type 0"));
  EXPECT_FALSE(simulator.execute("A 1 2 type"));
  EXPECT_FALSE(simulator.execute("A 1 2 type 4 5"));

  EXPECT_NE(et_simulator::HeapObject::theHeap.find(1),
            et_simulator::HeapObject::theHeap.end());
}

TEST(SimulatorTest, Parse_Object_Object_Update) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.execute("U 1 2 3 0"));
  EXPECT_FALSE(simulator.execute("U 1 2 3"));
  EXPECT_FALSE(simulator.execute("U 1 2"));
  EXPECT_FALSE(simulator.execute("U 1 2 3 4 5"));
}

TEST(SimulatorTest, Parse_Object_Method_Entry) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.execute("M 1 2 3"));
  EXPECT_FALSE(simulator.execute("M 1 2"));
  EXPECT_FALSE(simulator.execute("M 1 2 3 4"));
}

TEST(SimulatorTest, Executions) {
  et_simulator::Simulator simulator("", "");
  simulator.execute("M 474 0 1000");
  simulator.execute("A 21 16 type 1000");

  EXPECT_NE(et_simulator::HeapObject::theHeap.find(21),
            et_simulator::HeapObject::theHeap.end());

  et_simulator::HeapObject* heapObj = et_simulator::HeapObject::theHeap[21];
  et_simulator::CCNode* ccnode  = heapObj->getAllocCC();
  EXPECT_EQ(heapObj->getId(), 21);
  EXPECT_EQ(heapObj->getType(), "type");
  EXPECT_EQ(heapObj->getSize(), 16);

  EXPECT_EQ(ccnode->getMethodId(), 474);
  EXPECT_EQ(ccnode->getThreadId(), 1000);
}
