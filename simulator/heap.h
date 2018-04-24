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
  int id;
  std::string type;
  int bytes;
  bool live;

  int alloc_time;
  CCNode * alloc_cc;

  int death_time;
  CCNode * death_cc;

  // union/find stuff
  HeapObject * parent;
  int rank;
  HeapObject * points_to;

  int size;
  int num_dead;

 public:
  HeapObject(int i)
    : id(i),
      type("UNKNOWN"),
      bytes(-1),
      live(true),
      alloc_time(-1),
      alloc_cc(0),
      death_time(-1),
      death_cc(0),
      parent(0),
      rank(0),
      points_to(0),
      size(1),
      num_dead(0)
  {}

  int get_id() const { return id; }

  const std::string & get_type() const { return type; }

  void MarkAllocated(int a_time, int sz, const std::string & ty) {
    alloc_time = a_time;
    bytes = sz;
    type = ty;
  }

  void MarkDead(int d_time) {
    live = false;
    death_time = d_time;
  }

  bool is_live() const { return live; }

  int get_allocation_bytes() { return bytes; }

  int get_allocation_time() const { return alloc_time; }
  int get_death_time() const { return death_time; }

  CCNode * get_allocation_context() const { return alloc_cc; }
  void MarkAllocatedCC(CCNode * cc) { alloc_cc = cc; }

  CCNode * get_death_context() const { return death_cc; }
  void setDeathCC(CCNode * cc) { death_cc = cc; }

  void IncrementRank() { rank++; }
  int get_rank() const { return rank; }

  void set_parent(HeapObject * new_parent) {
    parent = new_parent;
  }

  HeapObject * get_parent() const { return parent; }
  bool is_root() const { return parent == 0; }

  HeapObject * get_points_to() const { return points_to; }

  void set_size(int new_size) { size = new_size; }
  int get_size() const { return size; }

  void set_number_of_dead_objects(int nd) { num_dead = nd; }
  void IncrementNumberOfDeadObjects() { num_dead++; }
  int get_number_of_dead_objects() const { return num_dead; }

  static HeapMap heap;

  // Global heap
  static HeapObject * DemandHeapObject(int object_id);

  // Disjoint sets operations

  static HeapObject * Find(HeapObject * obj);
  static HeapObject * Union(HeapObject * one, HeapObject * two);
  static HeapObject * RecUnion(HeapObject * one, HeapObject * two);

  void setPointsTo(HeapObject * target);
};
}  // namespace et_simulator
#endif  // ET2_SIMULATOR_SIMULATOR_HEAP_H
