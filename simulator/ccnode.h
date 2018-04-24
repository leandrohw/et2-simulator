#ifndef ET2_SIMULATOR_SIMULATOR_CCNODE_H_
#define ET2_SIMULATOR_SIMULATOR_CCNODE_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "simulator/method.h"
#include "absl/strings/str_join.h"

namespace et_simulator {
class CCNode
{
 public:
  typedef std::map<int64_t, CCNode *> CCNodeMap;
  typedef std::vector<CCNode *> CCNodeVector;

 private:
  int method_id_;
  int thread_id_;
  CCNode * parent_;
  CCNodeVector children_;

  int calls_;

  int first_call_;
  int last_call_;

  int allocated_bytes_;
  int allocated_objects_;
  int dead_bytes_;
  int dead_objects_;

  int total_allocated_bytes_;
  int total_allocated_objects_;
  int total_dead_bytes_;
  int total_dead_objects_;

  int allocated_rank_;

 public:
  static int count;

  CCNode(int id, int thread_id_, int time, CCNode * par)
    : method_id_(id),
      thread_id_(thread_id_),
      parent_(par),
      calls_(0),
      first_call_(time),
      last_call_(time),
      allocated_bytes_(0),
      allocated_objects_(0),
      dead_bytes_(0),
      dead_objects_(0),
      total_allocated_bytes_(0),
      total_allocated_objects_(0),
      total_dead_bytes_(0),
      total_dead_objects_(0),
      allocated_rank_(0)
  {
    count++;
  }

  // Fields

  CCNode * DemandChild(int id, int thread_id_, int time);

  int get_method_id_() const { return method_id_; }

  std::string GetMethodFullName();

  int get_thread_id_() const { return thread_id_; }

  CCNode * get_parent_() const { return parent_; }

  const CCNodeVector & get_children_() const { return children_; }


  // Accounting

  void ComputeTotals();
  void CollectNodes(CCNodeVector & all);
  void RankNodes();

  void IncrementCalls() { calls_++; }
  int get_calls_() const { return calls_; }

  void set_last_call_(int time) { last_call_ = time; }
  int get_last_call_() const { return last_call_; }
  int get_first_call_() const { return first_call_; }

  void IncrementAllocatedBytes(int bytes) { allocated_bytes_ += bytes; }
  int get_allocated_bytes() const { return allocated_bytes_; }

  void IncrementAllocatedObjects() { allocated_objects_++; }
  int get_allocated_objects() const { return allocated_objects_; }

  void IncrementDeadBytes(int bytes) { dead_bytes_ += bytes; }
  int get_dead_bytes_() const { return dead_bytes_; }

  void IncrementDeadObjects() { dead_objects_++; }
  int get_dead_objects_() const { return dead_objects_; }

  void IncrementTotalAllocatedBytes(int total) { total_allocated_bytes_ += total; }
  int get_total_allocated_bytes() const { return total_allocated_bytes_; }

  void IncrementTotalAllocatedObjects(int total) { total_allocated_objects_ += total; }
  int get_total_allocated_objects() const { return total_allocated_objects_; }

  void IncrementTotalDeadBytes(int total) { total_dead_bytes_ += total; }
  int get_total_dead_bytes_() const { return total_dead_bytes_; }

  void IncrementTotalDeadObjects(int total) { total_dead_objects_ += total; }
  int get_total_dead_objects_() const { return total_dead_objects_; }

  void set_allocated_rank(int rank) { allocated_rank_ = rank; }
  int get_allocated_rank() const { return allocated_rank_; }
};
}  // namespace et_simulator

#endif  // ET2_SIMULATOR_SIMULATOR_CCNODE_H_
