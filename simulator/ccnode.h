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
  int method_id;
  int thread_id;
  CCNode * parent;
  CCNodeVector children;

  int calls;

  int first_call;
  int last_call;

  int alloc_bytes;
  int alloc_objects;
  int dead_bytes;
  int dead_objects;

  int total_alloc_bytes;
  int total_alloc_objects;
  int total_dead_bytes;
  int total_dead_objects;

  int alloc_rank;

 public:
  static int count;

  CCNode(int id, int thread_id, int time, CCNode * par)
    : method_id(id),
      thread_id(thread_id),
      parent(par),
      calls(0),
      first_call(time),
      last_call(time),
      alloc_bytes(0),
      alloc_objects(0),
      dead_bytes(0),
      dead_objects(0),
      total_alloc_bytes(0),
      total_alloc_objects(0),
      total_dead_bytes(0),
      total_dead_objects(0),
      alloc_rank(0)
  {
    count++;
  }

  // Fields

  CCNode * DemandChild(int id, int thread_id, int time);

  int get_method_id() const { return method_id; }

  std::string GetMethodFullName();

  int get_thread_id() const { return thread_id; }

  CCNode * get_parent() const { return parent; }

  const CCNodeVector & get_children() const { return children; }


  // Accounting

  void ComputeTotals();
  void CollectNodes(CCNodeVector & all);
  void RankNodes();

  void IncrementCalls() { calls++; }
  int get_calls() const { return calls; }

  void set_last_call(int time) { last_call = time; }
  int get_last_call() const { return last_call; }
  int get_first_call() const { return first_call; }

  void IncrementAllocatedBytes(int bytes) { alloc_bytes += bytes; }
  int get_allocated_bytes() const { return alloc_bytes; }

  void IncrementAllocatedObjects() { alloc_objects++; }
  int get_allocated_objects() const { return alloc_objects; }

  void IncrementDeadBytes(int bytes) { dead_bytes += bytes; }
  int get_dead_bytes() const { return dead_bytes; }

  void IncrementDeadObjects() { dead_objects++; }
  int get_dead_objects() const { return dead_objects; }

  void IncrementTotalAllocatedBytes(int total) { total_alloc_bytes += total; }
  int get_total_allocated_bytes() const { return total_alloc_bytes; }

  void IncrementTotalAllocatedObjects(int total) { total_alloc_objects += total; }
  int get_total_allocated_objects() const { return total_alloc_objects; }

  void IncrementTotalDeadBytes(int total) { total_dead_bytes += total; }
  int get_total_dead_bytes() const { return total_dead_bytes; }

  void IncrementTotalDeadObjects(int total) { total_dead_objects += total; }
  int get_total_dead_objects() const { return total_dead_objects; }

  void set_allocated_rank(int rank) { alloc_rank = rank; }
  int get_allocated_rank() const { return alloc_rank; }
};
}  // namespace et_simulator

#endif  // ET2_SIMULATOR_SIMULATOR_CCNODE_H_
