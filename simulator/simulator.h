#ifndef SIMULATOR_H__
#define SIMULATOR_H__

#include <iostream>
#include <fstream>
#include "cctree.h"

class Simulator
{
 private:
  CCTree * tree;

  std::string tracefile;
  std::string namesfile;

  void read_name_file(std::ifstream & name_file);
  void read_trace_file(std::ifstream & in);

 public:
  Simulator(std::string tf, std::string nf) :
  tracefile(tf),
  namesfile(nf) {
    tree = new CCTree();
  }

  void simulate();



};

#endif
