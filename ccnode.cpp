#include "ccnode.h"

MethodMap Method::allMethods;
int CCNode::count = 0;

CCNode * CCNode::demand_child(int id, int thread_id, int time)
{
  for (CCNodeVector::iterator p = children.begin();
    p != children.end();
    p++)
  {
    CCNode * child = (*p);
    if (child->method_id == id) {
      child->setLastCall(time);
      return child;
    }
  }

  // Not Found
  CCNode * new_child = new CCNode(id, thread_id, time, this);
  children.push_back(new_child);
  return new_child;
}

void CCNode::computeTotals()
{
  // Start with this node's values
  incTotalAllocBytes(getAllocBytes());
  incTotalAllocObjects(getAllocObjects());
  incTotalDeadBytes(getDeadBytes());
  incTotalDeadObjects(getDeadObjects());

  // Compute totals for any children, then add up
  const CCNodeVector & children = getChildren();
  for (CCNodeVector::const_iterator p = children.begin();
       p != children.end();
       p++) {
      CCNode * child = (*p);
      child->computeTotals();

      incTotalAllocBytes(child->getTotalAllocBytes());
      incTotalAllocObjects(child->getTotalAllocObjects());
      incTotalDeadBytes(child->getTotalDeadBytes());
      incTotalDeadObjects(child->getTotalDeadObjects());
  }
}

std::string CCNode::getMethodFullName()
{
  Method * meth = Method::getMethod(getMethodId());
  std::string result;
  if (meth)
    result = meth->getClassName() + "::" + meth->getMethodName();
  else
    result = "ENTRY";

  return result;
}



void CCNode::collectNodes(CCNodeVector & all)
{
  all.push_back(this);
  const CCNodeVector & children = getChildren();
  for (CCNodeVector::const_iterator p = children.begin();
       p != children.end();
       p++)
    {
      CCNode * child = (*p);
      child->collectNodes(all);
    }
}

bool compareNodes(CCNode * one, CCNode * two)
{
  return one->getTotalAllocBytes() < two->getTotalAllocBytes();
}

void CCNode::rankNodes()
{
  CCNodeVector all;
  collectNodes(all);
  std::sort(all.begin(), all.end(), compareNodes);

  int rank = all.size();
  for (CCNodeVector::const_iterator p = all.begin();
       p != all.end();
       p++)
    {
      CCNode * child = (*p);
      child->setAllocRank(rank);
      rank--;
    }
}
