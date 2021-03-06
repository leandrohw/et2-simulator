#ifndef ET2_SIMULATOR_SIMULATOR_CCTREE_H_
#define ET2_SIMULATOR_SIMULATOR_CCTREE_H_

#include "simulator/ccnode.h"
#include "simulator/heap.h"

namespace et_simulator {
// Context tree.
class CCTree {
 private:
  CCNode * root_;
  std::map<int, int> thread_id_map_;

  //  Multi-threaded stack__
  typedef std::map<int, CCNode *> StackMap;
  StackMap stack__;
  //  Map from thread objects to the context in which start() was called
  StackMap thread_starts;

  //  Global counters
  int64_t total_allocated_size_ = 0;
  int64_t no_allocation_ = 0;

  int thread_start_method_id_ = 0;
  int last_method_id_ = 0;

  int64_t thread_number_ = 0;


  int depth_ = 0;
  int64_t time = 0;
  bool in_death_record_s_ = false;
  int64_t last_thread_id_ = 0;
  int64_t record_ = 0;

  void LogStack(CCNode * node);
 public:
  CCTree() {
    root_ = new CCNode(0, 0, 0, 0);
    last_thread_id_ = 0;
    stack__[0] = root_;
    thread_starts[0] = root_;
  }

  int get_depth() { return depth_; }
  int get_time() { return time; }
  int64_t get_thread_number() { return thread_number_; }
  int64_t get_last_thread_id() { return last_thread_id_; }

  void HandleObjectAllocation(int object_id, int size,
                              std::string type, int thread_id);
  void HandleObjectDeath(int object_id);
  void HandleObjectUpdate(int old_target, int object_id,
                          int new_target, int target_id);
  void HandleMethodEntry(int method_id, int object_id, int thread_id);
  void HandleMethodExit(int method_id, int object_id, int thread_id);

  //  Output methods

  void PrintTree(CCNode * node, int depth_);
  void EmitPath(CCNode * node, std::ofstream & out);
  void EmitTreeMapTM3(CCNode * node, std::ofstream & out);
  void EmitTreeMapTM3(std::ofstream & out);

  //  treeml output
  void EmitTreeML(CCNode * node, std::ofstream & out, int depth_);
  void emitTreeML(std::ofstream & out);

  //  JSON output
  void EmitTreeJSON(CCNode * node, std::ofstream & out, int depth_);
  void EmitTreeJSON(std::ofstream & out);
};
}  // namespace et_simulator
#endif  // ET2_SIMULATOR_SIMULATOR_CCTREE_H
