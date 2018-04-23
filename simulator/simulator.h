#ifndef ET2_SIMULATOR_SIMULATOR_SIMULATOR_H_
#define ET2_SIMULATOR_SIMULATOR_SIMULATOR_H_

#include <iostream>
#include <fstream>
#include "simulator/cctree.h"

// Heap simulator from traces generated by Elephant Tracks
//
// Event traces:
// * A: Object allocation
// * U: Object update
// * M: Method entry
// * E: Method exit
// * W: Object witness
//
namespace et_simulator {
class Simulator
{
 private:
  // Context tree
  CCTree * tree;

  // Name of the files
  std::string tracefile;
  std::string namesfile;

  // --------------
  // Helper methods
  // --------------

  // Handles input
  void read_name_file();
  void read_trace_file();

  // Handles output
  void report();

  // Parses and validates trace events
  bool parse_object_allocation(std::vector<std::string> trace);
  bool parse_object_update(std::vector<std::string> trace);
  bool parse_method_entry(std::vector<std::string> trace);
  bool parse_method_exit(std::vector<std::string> trace);

 public:
  Simulator(std::string tf, std::string nf) :
  tracefile(tf),
  namesfile(nf) {
    tree = new CCTree();
  }

  // Executes an individual trace event
  bool execute(std::string line);
  // Simulates the entire heap from a traces file
  void simulate();

  std::string getTraceFile() const { return tracefile; }
  std::string getNamesFile() const { return namesfile; }

  void setTraceFile(std::string tf) { tracefile = tf; }
  void setNamesFile(std::string nf) { namesfile = nf; }

};
}  //  namespace et_simulator
#endif  //  ET2_SIMULATOR_SIMULATOR_SIMULATOR_H_
