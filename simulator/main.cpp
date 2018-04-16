#include "simulator.h"

int main(int argc, char * argv[])
{
  if (argc < 3) {
    std::cout << "Usage: cctree <tracefile> <namesfile>" << std::endl;
    exit(0);
  }

  Simulator simulator(argv[1], argv[2]);
  simulator.simulate();
}
