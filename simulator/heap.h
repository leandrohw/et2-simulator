#ifndef ET2_SIMULATOR_SIMULATOR_HEAP_H_
#define ET2_SIMULATOR_SIMULATOR_HEAP_H_

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <cmath>
#include <algorithm>

#include "simulator/ccnode.h"


namespace et_simulator {
class HeapObject;

typedef std::map<int, HeapObject *> HeapMap;

class HeapObject
{
 private:
  int id_;
  std::string type_;
  int bytes_;
  bool live_;

  int allocated_time_;
  CCNode * allocated_context_;

  int death_time;
  CCNode * death_context_;

  // union/find stuff
  HeapObject * parent_;
  int rank;
  HeapObject * points_to_;

  int size_;
  int number_of_dead_;

 public:
  HeapObject(int i)
    : id_(i),
      type_("UNKNOWN"),
      bytes_(-1),
      live_(true),
      allocated_time_(-1),
      allocated_context_(0),
      death_time(-1),
      death_context_(0),
      parent_(0),
      rank(0),
      points_to_(0),
      size_(1),
      number_of_dead_(0)
  {}

  int get_id() const { return id_; }

  const std::string & get_type() const { return type_; }

  void MarkAllocated(int a_time, int sz, const std::string & ty) {
    allocated_time_ = a_time;
    bytes_ = sz;
    type_ = ty;
  }

  void MarkDead(int d_time) {
    live_ = false;
    death_time = d_time;
  }

  bool is_live() const { return live_; }

  int get_allocation_bytes() { return bytes_; }

  int get_allocation_time() const { return allocated_time_; }
  int get_death_time() const { return death_time; }

  CCNode * get_allocation_context() const { return allocated_context_; }
  void MarkAllocatedCC(CCNode * cc) { allocated_context_ = cc; }

  CCNode * get_death_context() const { return death_context_; }
  void setDeathCC(CCNode * cc) { death_context_ = cc; }

  void IncrementRank() { rank++; }
  int get_rank() const { return rank; }

  void set_parent(HeapObject * new_parent) {
    parent_ = new_parent;
  }

  HeapObject * get_parent() const { return parent_; }
  bool is_root() const { return parent_ == 0; }

  HeapObject * get_points_to() const { return points_to_; }

  void set_size(int new_size) { size_ = new_size; }
  int get_size() const { return size_; }

  void set_number_of_dead_objects(int nd) { number_of_dead_ = nd; }
  void IncrementNumberOfDeadObjects() { number_of_dead_++; }
  int get_number_of_dead_objects() const { return number_of_dead_; }

  static HeapMap heap;

  // Global heap
  static HeapObject * DemandHeapObject(int object_id);

  // Disjoint sets operations

  static HeapObject * Find(HeapObject * obj);
  static HeapObject * Union(HeapObject * one, HeapObject * two);
  static HeapObject * RecUnion(HeapObject * one, HeapObject * two);

  void set_points_to(HeapObject * target);
};
}  // namespace et_simulator
#endif  // ET2_SIMULATOR_SIMULATOR_HEAP_H
