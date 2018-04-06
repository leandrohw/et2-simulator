
#ifndef CCTREE_H
#define CCTREE_H

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdlib.h>

using namespace std;

class Method;
class CCNode;

typedef map<long, CCNode *> CCNodeMap;
typedef map<int, Method *> MethodMap;
typedef vector<CCNode *> CCNodeVector;

class Method
{
private:

  int id;
  string class_name;
  string method_name;
  string signature;

  static MethodMap allMethods;

public:

  Method(int i, string cn, string mn, string sig)
    : id(i),
      class_name(cn),
      method_name(mn),
      signature(sig)
  {
    allMethods[i] = this;
  }

  const string & getClassName() const { return class_name; }
  const string & getMethodName() const { return method_name; }
  const string & getSignature() const { return signature; }

  static Method * getMethod(int id) {
    return allMethods[id];
  }
};


class CCNode
{
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

  // -- Fields

  CCNode * demand_child(int id, int thread_id, int time);

  int getMethodId() const { return method_id; }
  string getMethodFullName();

  int getThreadId() const { return thread_id; }

  CCNode * getParent() const { return parent; }

  const CCNodeVector & getChildren() const { return children; }

  // --- Output methods

  void printTree(int depth);
  void printStack();

  //     tm3 output
  void emitPath(ofstream & out);
  void emitTreeMapTM3Rec(ofstream & out);
  void emitTreeMapTM3(ofstream & out);

  //     treeml output
  void emitTreeMLRec(ofstream & out, int depth);
  void emitTreeML(ofstream & out);

  //     JSON output
  void emitTreeJSONRec(ofstream & out, int depth);
  void emitTreeJSON(ofstream & out);

  // -- Accounting

  void computeTotals();
  void collectNodes(CCNodeVector & all);
  void rankNodes();

  void incCalls() { calls++; }
  int getCalls() const { return calls; }

  void setLastCall(int time) { last_call = time; }
  int getLastCall() const { return last_call; }
  int getFirstCall() const { return first_call; }

  void incAllocBytes(int bytes) { alloc_bytes += bytes; }
  int getAllocBytes() const { return alloc_bytes; }

  void incAllocObjects() { alloc_objects++; }
  int getAllocObjects() const { return alloc_objects; }

  void incDeadBytes(int bytes) { dead_bytes += bytes; }
  int getDeadBytes() const { return dead_bytes; }

  void incDeadObjects() { dead_objects++; }
  int getDeadObjects() const { return dead_objects; }

  void incTotalAllocBytes(int total) { total_alloc_bytes += total; }
  int getTotalAllocBytes() const { return total_alloc_bytes; }

  void incTotalAllocObjects(int total) { total_alloc_objects += total; }
  int getTotalAllocObjects() const { return total_alloc_objects; }

  void incTotalDeadBytes(int total) { total_dead_bytes += total; }
  int getTotalDeadBytes() const { return total_dead_bytes; }

  void incTotalDeadObjects(int total) { total_dead_objects += total; }
  int getTotalDeadObjects() const { return total_dead_objects; }

  void setAllocRank(int rank) { alloc_rank = rank; }
  int getAllocRank() const { return alloc_rank; }
};

#endif
