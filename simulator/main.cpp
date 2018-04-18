#include "gflags/gflags.h"
#include "glog/logging.h"
#include "simulator/simulator.h"

DEFINE_string(tracefile, "", "Trace file produced by Elephant Tracks");
DEFINE_string(namesfile, "", "Names file produced by Elephant Tracks");

int main(int argc, char * argv[])
{
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  et_simulator::Simulator simulator(FLAGS_tracefile, FLAGS_namesfile);
  simulator.simulate();
}
