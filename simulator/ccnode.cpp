#include "simulator/ccnode.h"

namespace et_simulator {
MethodMap Method::allMethods;
int CCNode::count = 0;

CCNode * CCNode::DemandChild(int id, int thread_id, int time)
{
  for (CCNodeVector::iterator p = children.begin();
    p != children.end();
    p++)
  {
    CCNode * child = (*p);
    if (child->method_id == id) {
      child->set_last_call(time);
      return child;
    }
  }

  // Not Found
  CCNode * new_child = new CCNode(id, thread_id, time, this);
  children.push_back(new_child);
  return new_child;
}

void CCNode::ComputeTotals()
{
  // Start with this node's values
  IncrementTotalAllocatedBytes(get_allocated_bytes());
  IncrementTotalAllocatedObjects(get_allocated_objects());
  IncrementTotalDeadBytes(get_dead_bytes());
  IncrementTotalDeadObjects(get_dead_objects());

  // Compute totals for any children, then add up
  const CCNodeVector & children = get_children();
  for (CCNodeVector::const_iterator p = children.begin();
       p != children.end();
       p++) {
      CCNode * child = (*p);
      child->ComputeTotals();

      IncrementTotalAllocatedBytes(child->get_total_allocated_objects());
      IncrementTotalAllocatedObjects(child->get_total_allocated_objects());
      IncrementTotalDeadBytes(child->get_total_dead_bytes());
      IncrementTotalDeadObjects(child->get_total_dead_objects());
  }
}

std::string CCNode::GetMethodFullName()
{
  Method * meth = Method::getMethod(get_method_id());
  std::string result;
  if (meth)
    result = meth->getClassName() + "::" + meth->getMethodName();
  else
    result = "ENTRY";

  return result;
}



void CCNode::CollectNodes(CCNodeVector & all)
{
  all.push_back(this);
  const CCNodeVector & children = get_children();
  for (CCNodeVector::const_iterator p = children.begin();
       p != children.end();
       p++)
    {
      CCNode * child = (*p);
      child->CollectNodes(all);
    }
}

bool compareNodes(CCNode * one, CCNode * two)
{
  return one->get_total_allocated_objects() < two->get_total_allocated_objects();
}

void CCNode::RankNodes()
{
  CCNodeVector all;
  CollectNodes(all);
  std::sort(all.begin(), all.end(), compareNodes);

  int rank = all.size();
  for (CCNodeVector::const_iterator p = all.begin();
       p != all.end();
       p++)
    {
      CCNode * child = (*p);
      child->set_allocated_rank(rank);
      rank--;
    }
}
}  // namespace et_simulator
