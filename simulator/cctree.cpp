#include "simulator/cctree.h"
#include "simulator/method.h"
#include "glog/logging.h"

namespace et_simulator {
int debug = 0;
void CCTree::handle_object_allocation(int object_id, int size,
                                      std::string type, int thread_id,
                                      int method_id) {
  CCNode * curContext = 0;

  if (thread_id == object_id) {
    // Spawning a new thread
    // Make it a child of the root
    curContext = root;
    CCNode * newContext = curContext->demand_child(method_id,
                                                   thread_id,
                                                   time);
    newContext->incCalls();
    theStack[thread_id] = newContext;
    LOG(INFO) << "Spawn thread 0x" << std::hex << thread_id
              << std::dec;
  }

  if (theStack[thread_id] == 0) {
    theStack[thread_id] = root;
  }

  curContext = theStack[thread_id];

  curContext->incAllocBytes(size);
  curContext->incAllocObjects();

  HeapObject * heapObject = HeapObject::DemandHeapObject(object_id);
  heapObject->setAlloc(time, size, type);
  heapObject->setAllocCC(curContext);

  LOG(INFO) <<  "Allocate 0x" << std::hex << object_id << " size 0x" << size << std::dec << " at time " << time;
  logStack(curContext);

  last_thread_id = thread_id;
}

void CCTree::handle_object_death(int object_id) {
  HeapObject * heapObject = HeapObject::DemandHeapObject(object_id);
  heapObject->setDead(time);
  (HeapObject::Find(heapObject))->incNumDead();

  CCNode * curContext = theStack[last_thread_id];

  curContext->incDeadBytes(heapObject->getSize());
  curContext->incDeadObjects();
  heapObject->setDeathCC(curContext);
}

void CCTree::handle_object_update(int old_target, int object_id,
                                  int new_target, int target_id) {
  if (object_id != 0 && new_target != 0) {
    HeapObject * heapObject = HeapObject::DemandHeapObject(object_id);
    HeapObject * targetObject = HeapObject::DemandHeapObject(new_target);
    HeapObject * troot = HeapObject::Find(targetObject);
    if (targetObject->getType() != "[C" &&
        targetObject->getType() != "UNKNOWN" &&
        troot->getId() != 0x19a) {

      LOG(INFO) << "Pointer from 0x" << std::hex << object_id << " "
                << heapObject->getType() << " to 0x" << new_target
                << std::dec << " " << targetObject->getType() << " at time "
                << time;

      CCNode * curContext = theStack[last_thread_id];
      logStack(curContext);

      heapObject->setPointsTo(targetObject);
    }
  }
}

void CCTree::handle_method_entry(int method_id, int object_id, int thread_id) {
  CCNode * curContext = theStack[thread_id];
  bool new_thread = false;
  if (curContext == 0) {
    // Spawning a new thread -- look up where the thread started.
    // Relies on the fact that the thread_id is the same as the
    // object_id of the Thread object instance.
    new_thread = true;
    curContext = threadStarts[thread_id];
    if (curContext) {
      LOG(INFO) << "Spawn thread 0x" << std::hex << thread_id << std::dec
                << " -- started at " << curContext->getMethodFullName()
                << " in context";
      logStack(curContext);
    } else {
      curContext = root;
      LOG(INFO) << "Problem: no threadStart for thread id 0x" << std::hex
                << thread_id << std::dec;
    }
  }


  time++;
  depth++;

  curContext = curContext->demand_child(method_id, thread_id, time);
  curContext->incCalls();
  theStack[thread_id] = curContext;

  if (new_thread) {
    LOG(INFO) << "Enter " << curContext->getMethodFullName() << " 0x"
               << std::hex << method_id << " thread 0x" << thread_id << std::dec
               << " at time " << time;
      logStack(curContext);
  }

  if (method_id == thread_start_method_id) {
    // Found a new thread start
    threadStarts[object_id] = curContext;
    thread_number++;
    threadIdNumbering[object_id] = thread_number;
    LOG(INFO) << "Found Thread.start at ";
    logStack(curContext);
  }

  last_thread_id = thread_id;
}

void CCTree::handle_method_exit(int method_id, int object_id, int thread_id) {
  CCNode * curContext = theStack[thread_id];
  if (theStack.find(thread_id) == theStack.end()) {
    std::cout << "Never seen thread 0x" << std::hex << thread_id;
  }

  if (curContext == 0) {
    Method * meth = Method::getMethod(method_id);
    LOG(FATAL) << "ERROR: Thread 0x" << std::hex << thread_id
               << " returning from method " << meth->getMethodName()
               << " in class " << meth->getClassName();
  }

  LOG(INFO) << "Exit  " << curContext->getMethodFullName() << " 0x"
            << std::hex << method_id << " thread 0x" << thread_id << std::dec
            << " at time " << time;
  logStack(curContext);

  time++;
  CCNode * returning = curContext;
  int cur_id = returning->getMethodId();
  int orig_depth = depth;
  while (returning && returning->getMethodId() != method_id) {
    returning = returning->getParent();
    depth--;
  }

  if (returning == 0) {
    LOG(INFO) << "THIS IS BAD: looking for "
              << std::hex << "0x" << method_id << " but found 0x"
              << cur_id << std::dec;
    logStack(returning);
    // current_node unchanged
    depth = orig_depth;
  } else {
    returning->setLastCall(time);
    theStack[thread_id] = returning->getParent();
    depth--;
  }
  last_thread_id = thread_id;
}


void CCTree::printTree(CCNode* node, int depth)
{
  if (node->getTotalAllocBytes() == 0 &&
      node->getTotalAllocObjects() == 0)
    return;

  for (int i = 0; i < depth; i++)
    std::cout << ".";

  if (depth > 0)
    std::cout << " ";

  std::cout << node->getMethodFullName() << " ";

  std::cout << node->getTotalAllocBytes() << " "
       << node->getAllocRank() << " "
       << node->getTotalAllocObjects() << "  "
       << node->getCalls() << std::endl;

  const CCNode::CCNodeVector & children = node->getChildren();
  for (CCNode::CCNodeVector::const_iterator p = children.begin();
       p != children.end();
       p++)
    {
      CCNode * child = (*p);
      printTree(child, depth+1);
    }
}

void CCTree::logStack(CCNode* node)
{
  CCNode * cur = node;
  while (cur) {
    LOG(INFO) << "  " << cur->getMethodFullName()
              << "(0x" << std::hex << cur->getMethodId()
              << std::dec << ")" << std::endl;

    cur = cur->getParent();
  }
}

void CCTree::emitPath(CCNode* node, std::ofstream & out)
{
  CCNode * parent = node->getParent();
  if (parent != 0)
    emitPath(parent, out);

  out << "\t" << node->getMethodFullName();
}

void CCTree::emitTreeMapTM3Rec(CCNode* node, std::ofstream & out)
{
  int abytes = node->getTotalAllocBytes();
  int dbytes = node->getTotalDeadBytes();

  if (abytes > 0 || dbytes > 0) {
    const CCNode::CCNodeVector & children = node->getChildren();

    // Compute total size of children
    int child_total_alloc = 0;
    int child_total_dead = 0;
    int num_children = 0;
    for (CCNode::CCNodeVector::const_iterator p = children.begin();
      p != children.end();
      p++)
    {
      CCNode * child = (*p);
      int child_alloc = child->getTotalAllocBytes();
      int child_dead =  child->getTotalDeadBytes();
      child_total_alloc += child_alloc;
      child_total_dead  += child_dead;
      if (child_alloc > 0 || child_dead > 0)
        num_children++;
    }

    // Output entry for this node
    out << (abytes+dbytes) << "\t";
    out << node->getCalls() << "\t";
    out << "0" << "\t";
    out << node->getThreadId() << "\t";
    out << node->getFirstCall() << "\t";
    emitPath(node, out);
    out << std::endl;

    // Output synthetic node representing the this node's contribution
    // (if necessary)
    if (num_children > 0) {
      int alloc_diff = abytes - child_total_alloc;
      int dead_diff = dbytes - child_total_dead;
      if (alloc_diff > 0) {
        out << alloc_diff << "\t";
        out << node->getCalls() << "\t";
        out << "0" << "\t";
        out << node->getThreadId() << "\t";
        out << node->getFirstCall() << "\t";
        emitPath(node, out);
        out << "\talloc";
        out << std::endl;
      }
      if (dead_diff > 0) {
        out << dead_diff << "\t";
        out << node->getCalls() << "\t";
        out << "0" << "\t";
        out << node->getThreadId() << "\t";
        out << node->getFirstCall() << "\t";
        emitPath(node, out);
        out << "\tdead";
        out << std::endl;
      }
    }
    // Output the children
    for (CCNode::CCNodeVector::const_iterator p = children.begin();
      p != children.end();
      p++)
    {
      CCNode * child = (*p);
      emitTreeMapTM3Rec(child, out);
    }
  }
}

void CCTree::emitTreeMapTM3(std::ofstream & out)
{
  out << "Bytes\tCalls\tLifetime\tThread\tFirst call" << std::endl;
  out << "INTEGER\tINTEGER\tINTEGER\tINTEGER\tINTEGER" << std::endl;

  emitTreeMapTM3Rec(root, out);
}

// Emit the calling context tree in TreeML format, for use in prefuse
void CCTree::emitTreeML(std::ofstream & out)
{
  out << "<tree>" << std::endl;
  out << " <declarations>" << std::endl;
  out << "  <attributeDecl name=\"name\" type=\"std::string\"/>" << std::endl;
  out << "  <attributeDecl name=\"class\" type=\"std::string\"/>" << std::endl;
  out << "  <attributeDecl name=\"bytes\" type=\"Long\"/>" << std::endl;
  out << "  <attributeDecl name=\"alloc\" type=\"Long\"/>" << std::endl;
  out << "  <attributeDecl name=\"dead\" type=\"Long\"/>" << std::endl;
  out << "  <attributeDecl name=\"objects\" type=\"Long\"/>" << std::endl;
  out << "  <attributeDecl name=\"objalloc\" type=\"Long\"/>" << std::endl;
  out << "  <attributeDecl name=\"objdead\" type=\"Long\"/>" << std::endl;
  out << "  <attributeDecl name=\"thread\" type=\"Long\"/>" << std::endl;
  out << " </declarations>" << std::endl;

  emitTreeMLRec(root, out, 1);

  out << "</tree>" << std::endl;
}

void CCTree::emitTreeMLRec(CCNode * node, std::ofstream & out, int depth)
{
  int abytes = node->getTotalAllocBytes();
  int dbytes = node->getTotalDeadBytes();

  if (abytes + dbytes > 0) {
    const CCNode::CCNodeVector & children = node->getChildren();
    bool is_leaf = (children.size() == 0);

    if (is_leaf) {
      // -- Leaf
      out << "<leaf>" << std::endl;
    } else {
      out << "<branch>" << std::endl;
    }

    Method * meth = Method::getMethod(node->getMethodId());
    std::string nm;
    std::string classnm;
    if (meth) {
      nm = meth->getMethodName();
      classnm = meth->getClassName();
    } else {
      nm = "ENTRY";
      classnm = "";
    }

    unsigned int i = nm.find("<init>");
    if (i != std::string::npos)
      nm.replace(i, i+6, "&lt;init&gt;");
    unsigned int j = nm.find("<clinit>");
    if (j != std::string::npos)
      nm.replace(j, j+8, "&lt;clinit&gt;");
    out << "<attribute name=\"name\" value=\"" << nm << "\"/>" << std::endl;
    out << "<attribute name=\"class\" value=\"" << classnm << "\"/>"
        << std::endl;
    out << "<attribute name=\"bytes\" value=\"" << (abytes+dbytes)
        << "\"/>" << std::endl;
    out << "<attribute name=\"alloc\" value=\"" << abytes << "\"/>"
        << std::endl;
    out << "<attribute name=\"dead\" value=\"" << dbytes << "\"/>"
        << std::endl;
    out << "<attribute name=\"objects\" value=\""
        << (node->getAllocObjects() + node->getDeadObjects()) << "\"/>"
        << std::endl;
    out << "<attribute name=\"objalloc\" value=\"" << node->getAllocObjects()
        << "\"/>" << std::endl;
    out << "<attribute name=\"objdead\" value=\"" << node->getDeadObjects()
        << "\"/>" << std::endl;
    out << "<attribute name=\"thread\" value=\""
        << threadIdNumbering[node->getThreadId()] << "\"/>" << std::endl;

    for (CCNode::CCNodeVector::const_iterator p = children.begin();
      p != children.end();
      p++)
    {
      CCNode * child = (*p);
      emitTreeMLRec(child, out, depth+1);
    }

    if (is_leaf) {
      // -- Leaf
      out << "</leaf>" << std::endl;
    } else {
      out << "</branch>" << std::endl;
    }
  }
}

void CCTree::emitTreeJSON(std::ofstream & out)
{
  emitTreeJSONRec(root, out, 0);
}

void CCTree::emitTreeJSONRec(CCNode* node, std::ofstream & out, int depth)
{
  int abytes = node->getTotalAllocBytes();
  int dbytes = node->getTotalDeadBytes();

  if (abytes + dbytes > 0) {
    std::string space(depth*2, ' ');
    out << space << "{" << std::endl;

    Method * meth = Method::getMethod(node->getMethodId());
    std::string nm;
    std::string classnm;
    if (meth) {
      nm = meth->getMethodName();
      classnm = meth->getClassName();
    } else {
      nm = "ENTRY";
      classnm = "";
    }

    out << space << " \"name\": \"" << nm << "\"," << std::endl;

    const CCNode::CCNodeVector & children = node->getChildren();
    if (children.size() == 0) {
      out << space << " \"size\": " << abytes << std::endl;
    } else {
      out << space << " \"children\": [" << abytes << std::endl;
      for (CCNode::CCNodeVector::const_iterator p = children.begin();
           p != children.end();
           p++)
      {
        CCNode * child = (*p);
        emitTreeJSONRec(child, out, depth+1);
      }
      out << space << " ]" << std::endl;
    }
    out << space << "}" << std::endl;
  }
}
}  // namespace et_simulator
