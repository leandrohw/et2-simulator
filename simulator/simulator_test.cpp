#include "simulator/simulator.h"
#include "gtest/gtest.h"

TEST(SimulatorTest, Constructor) {
  et_simulator::Simulator simulator("", "");
  EXPECT_EQ(simulator.get_name_file(), "");
  EXPECT_EQ(simulator.get_trace_file(), "");
}

TEST(SimulatorTest, Parse_Object_Allocation_Event) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.Execute("A 1 2 type 0"));
  EXPECT_FALSE(simulator.Execute("A 1 2 type"));
  EXPECT_FALSE(simulator.Execute("A 1 2 type 4 5"));

  EXPECT_NE(et_simulator::HeapObject::heap.find(1),
            et_simulator::HeapObject::heap.end());
}

TEST(SimulatorTest, Parse_Object_Object_Update) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.Execute("U 1 2 3 0"));
  EXPECT_FALSE(simulator.Execute("U 1 2 3"));
  EXPECT_FALSE(simulator.Execute("U 1 2"));
  EXPECT_FALSE(simulator.Execute("U 1 2 3 4 5"));
}

TEST(SimulatorTest, Parse_Object_Method_Entry) {
  et_simulator::Simulator simulator("", "");
  EXPECT_TRUE(simulator.Execute("M 1 2 3"));
  EXPECT_FALSE(simulator.Execute("M 1 2"));
  EXPECT_FALSE(simulator.Execute("M 1 2 3 4"));
}

TEST(SimulatorTest, Executions) {
  et_simulator::Simulator simulator("", "");
  simulator.Execute("M 474 0 1000");
  simulator.Execute("A 21 16 type 1000");

  EXPECT_NE(et_simulator::HeapObject::heap.find(21),
            et_simulator::HeapObject::heap.end());

  et_simulator::HeapObject* heapObj = et_simulator::HeapObject::heap[21];
  et_simulator::CCNode* ccnode  = heapObj->get_allocation_context();
  EXPECT_EQ(heapObj->get_id(), 21);
  EXPECT_EQ(heapObj->get_type(), "type");
  EXPECT_EQ(heapObj->get_allocation_bytes(), 16);

  EXPECT_EQ(ccnode->get_method_id(), 474);
  EXPECT_EQ(ccnode->get_thread_id(), 1000);
}
