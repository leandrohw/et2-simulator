#include "simulator/heap.h"

namespace et_simulator {

HeapMap HeapObject::theHeap;

HeapObject * HeapObject::Find(HeapObject * obj)
{
  HeapObject * parent = obj->getParent();
  if (parent == 0)
    return obj;

  HeapObject * root = Find(parent);
  obj->setParent(root);

  return root;
}

HeapObject * HeapObject::Union(HeapObject * one, HeapObject * two)
{
  HeapObject * one_root = Find(one);
  HeapObject * two_root = Find(two);

  if (one_root == two_root)
    return one_root;

  int new_size = one_root->getSize() + two_root->getSize();
  int new_nd = one_root->getNumDead() + two_root->getNumDead();

  int one_rank = one_root->getRank();
  int two_rank = two_root->getRank();

  HeapObject * new_root = 0;
  if (one_rank < two_rank) {
    // Two becomes root
    one_root->setParent(two_root);
    two_root->setSize(new_size);
    two_root->setNumDead(new_nd);
    new_root = two_root;
  } else {
    if (one_rank > two_rank) {
      // One becomes root
      two_root->setParent(one_root);
      one_root->setSize(new_size);
      one_root->setNumDead(new_nd);
      new_root = one_root;
    } else {
      // Pick one, and increment rank
      two_root->setParent(one_root);
      one_root->setSize(new_size);
      one_root->incRank();
      one_root->setNumDead(new_nd);
      new_root = one_root;
    }
  }

  return new_root;
}

// Recursive union (for model that includes pointers)
HeapObject * HeapObject::RecUnion(HeapObject * one, HeapObject * two)
{
  HeapObject * one_root = Find(one);
  HeapObject * two_root = Find(two);

  if (one_root == two_root)
    return one_root;

  HeapObject * one_ptr = one->getPointsTo();
  HeapObject * two_ptr = two->getPointsTo();

  HeapObject * new_root = Union(one, two);

  if (one_ptr == 0 && two_ptr == 0)
    return new_root;

  HeapObject * new_ptr = 0;
  if (one_ptr && two_ptr) {
    new_ptr = RecUnion(one_ptr, two_ptr);
  } else {
    if (one_ptr) {
      new_ptr = one_ptr;
    } else {
      if (two_ptr) {
        new_ptr = two_ptr;
      }
    }
  }

  if (new_ptr) {
    HeapObject * new_root_2 = Find(new_root);
    new_root_2->pointsTo = new_ptr;
  }

  return new_root;
}

void HeapObject::setPointsTo(HeapObject * target)
{
  HeapObject * root = Find(this);
  HeapObject * old_ptr = root->getPointsTo();
  HeapObject * new_ptr = 0;
  if (old_ptr) {
    new_ptr = RecUnion(old_ptr, target);
  } else {
    new_ptr = target;
  }

  HeapObject * new_root = Find(root);
  new_root->pointsTo = new_ptr;
}

// Heap management
HeapObject * HeapObject::DemandHeapObject(int object_id)
{
  HeapObject * result;
  HeapMap::iterator f = theHeap.find(object_id);
  if (f != theHeap.end()) {
    result = (*f).second;
  } else {
    result = new HeapObject(object_id);
    theHeap[object_id] = result;
  }

  return result;
}
}  // namespace et_simulator
