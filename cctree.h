
#ifndef CCTREE_H
#define CCTREE_H

#include "ccnode.h"
#include "heap.h"

class CCTree {

 private:

  HeapObject * heapObject;
  HeapObject * targetObject;
  CCNode * root;
  std::map<int, int> threadIdNumbering;



  // -- Multi-threaded stack
  typedef std::map<int, CCNode *> StackMap;
  StackMap theStack;
  // -- Global counters

  int64_t total_alloc_size = 0;

  int64_t no_alloc = 0;



  //    Map from thread objects to the context in which start() was called
  StackMap threadStarts;
  int thread_start_method_id = 0;
  int thread_number = 0;


  int depth = 0;
  int64_t time = 0;
  bool in_death_records = false;
int64_t last_thread_id = 0;
  int64_t record = 0;
 public:

  CCTree() {
    root = new CCNode(0, 0, 0, 0);

    last_thread_id = 0;
    theStack[0] = root;
    threadStarts[0] = root;


  }
  void handle_object_allocation(int object_id, int size, std::string type, int thread_id, int method_id);
  void handle_object_death(int object_id);
  void handle_object_update(int old_target, int object_id, int new_target, int target_id);
  void handle_method_entry(int method_id, int object_id, int thread_id);
  void handle_method_exit(int method_id, int object_id, int thread_id);
};

#endif
