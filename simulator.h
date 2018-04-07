#ifndef SIMULATOR_H__
#define SIMULATOR_H__

#include "cctree.h"
#include "heap.h"

class Simulator
{
 private:
  std::string tracefile;
  std::string namesfile;

  void read_name_file(std::ifstream & name_file);
  void read_trace_file(std::ifstream & in, CCNode * root);

  void handle_object_allocation();
  void handle_object_death();
  void handle_object_update();
  void handle_method_entry();
  void handle_method_exit();

  int64_t last_thread_id;

 public:
  Simulator(std::string tf, std::string nf) :
  tracefile(tf),
  namesfile(tf) {}

  void simulate();

  // -- Global counters

  int64_t total_alloc_size = 0;
  int64_t record_count = 0;
  int64_t no_alloc = 0;

  // -- Multi-threaded stack
  typedef std::map<int, CCNode *> StackMap;
  StackMap theStack;

  //    Map from thread objects to the context in which start() was called
  StackMap threadStarts;
  int thread_start_method_id = 0;
  int thread_number = 0;
  std::map<int, int> threadIdNumbering;
};

#endif
