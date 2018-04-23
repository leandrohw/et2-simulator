#include "simulator/cctree.h"
#include "simulator/method.h"
#include "glog/logging.h"

namespace et_simulator {
int debug = 0;
void CCTree::HandleObjectAllocation(int object_id, int size,
                                    std::string type, int thread_id) {
  CCNode * curContext = 0;

  if (thread_id == object_id) {
    // Spawning a new thread
    // Make it a child of the root
    curContext = root;
    CCNode * newContext = curContext->DemandChild(last_method_id,
                                                  thread_id,
                                                  time);
    newContext->IncrementCalls();
    theStack[thread_id] = newContext;
    LOG(INFO) << "Spawn thread 0x" << std::hex << thread_id
              << std::dec;
  }

  if (theStack[thread_id] == 0) {
    theStack[thread_id] = root;
  }

  curContext = theStack[thread_id];

  curContext->IncrementAllocatedBytes(size);
  curContext->IncrementAllocatedObjects();

  HeapObject * heapObject = HeapObject::DemandHeapObject(object_id);
  heapObject->setAlloc(time, size, type);
  heapObject->setAllocCC(curContext);

  LOG(INFO) <<  "Allocate 0x" << std::hex << object_id << " size 0x" << size << std::dec << " at time " << time;
  LogStack(curContext);

  last_thread_id = thread_id;
}

void CCTree::HandleObjectDeath(int object_id) {
  HeapObject * heapObject = HeapObject::DemandHeapObject(object_id);
  heapObject->setDead(time);
  (HeapObject::Find(heapObject))->incNumDead();

  CCNode * curContext = theStack[last_thread_id];

  curContext->IncrementDeadBytes(heapObject->getSize());
  curContext->IncrementDeadObjects();
  heapObject->setDeathCC(curContext);
}

void CCTree::HandleObjectUpdate(int old_target, int object_id,
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
      LogStack(curContext);

      heapObject->setPointsTo(targetObject);
    }
  }
}

void CCTree::HandleMethodEntry(int method_id, int object_id, int thread_id) {
  CCNode * curContext = theStack[thread_id];
  last_method_id = method_id;
  if (curContext == 0) {
    // Spawning a new thread -- look up where the thread started.
    // Relies on the fact that the thread_id is the same as the
    // object_id of the Thread object instance.
    curContext = threadStarts[thread_id];
    if (curContext) {
      LOG(INFO) << "Spawn thread 0x" << std::hex << thread_id << std::dec
                << " -- started at " << curContext->GetMethodFullName()
                << " in context";
      LogStack(curContext);
    } else {
      curContext = root;
      LOG(INFO) << "Problem: no threadStart for thread id 0x" << std::hex
                << thread_id << std::dec;
    }
  }

  time++;
  depth++;

  curContext = curContext->DemandChild(method_id, thread_id, time);
  curContext->IncrementCalls();
  theStack[thread_id] = curContext;

  LOG(INFO) << "Enter " << curContext->GetMethodFullName() << " 0x"
            << std::hex << method_id << " thread 0x" << thread_id << std::dec
            << " at time " << time;
  LogStack(curContext);

  if (method_id == thread_start_method_id) {
    // Found a new thread start
    threadStarts[object_id] = curContext;
    thread_number++;
    threadIdNumbering[object_id] = thread_number;
    LOG(INFO) << "Found Thread.start at ";
    LogStack(curContext);
  }

  last_thread_id = thread_id;
}

void CCTree::HandleMethodExit(int method_id, int object_id, int thread_id) {
  CCNode * curContext = theStack[thread_id];
  last_method_id = method_id;

  if (theStack.find(thread_id) == theStack.end()) {
    LOG(INFO) << "Never seen thread 0x" << std::hex << thread_id;
  }

  if (curContext == 0) {
    Method * meth = Method::getMethod(method_id);
    LOG(FATAL) << "ERROR: Thread 0x" << std::hex << thread_id
               << " returning from method " << meth->getMethodName()
               << " in class " << meth->getClassName();
  }

  LOG(INFO) << "Exit  " << curContext->GetMethodFullName() << " 0x"
            << std::hex << method_id << " thread 0x" << thread_id << std::dec
            << " at time " << time;
  LogStack(curContext);

  time++;
  CCNode * returning = curContext;
  int cur_id = returning->get_method_id();
  int orig_depth = depth;
  while (returning && returning->get_method_id() != method_id) {
    returning = returning->get_parent();
    depth--;
  }

  if (returning == 0) {
    LOG(INFO) << "THIS IS BAD: looking for "
              << std::hex << "0x" << method_id << " but found 0x"
              << cur_id << std::dec;
    LogStack(returning);
    // current_node unchanged
    depth = orig_depth;
  } else {
    returning->set_last_call(time);
    theStack[thread_id] = returning->get_parent();
    depth--;
  }
  last_thread_id = thread_id;
}


void CCTree::PrintTree(CCNode* node, int depth)
{
  if (node->get_total_allocated_objects() == 0 &&
      node->get_total_allocated_bytes() == 0)
    return;

  for (int i = 0; i < depth; i++)
    std::cout << ".";

  if (depth > 0)
    std::cout << " ";

  std::cout << node->GetMethodFullName() << " ";

  std::cout << node->get_total_allocated_bytes() << " "
            << node->get_allocated_rank() << " "
            << node->get_total_allocated_objects() << "  "
            << node->get_calls() << std::endl;

  const CCNode::CCNodeVector & children = node->get_children();
  for (CCNode::CCNodeVector::const_iterator p = children.begin();
       p != children.end();
       p++)
  {
    CCNode * child = (*p);
    PrintTree(child, depth+1);
  }
}

void CCTree::LogStack(CCNode* node)
{
  CCNode * cur = node;
  while (cur) {
    LOG(INFO) << "  " << cur->GetMethodFullName()
              << "(0x" << std::hex << cur->get_method_id()
              << std::dec << ")";

    cur = cur->get_parent();
  }
}

void CCTree::EmitPath(CCNode* node, std::ofstream & out)
{
  CCNode * parent = node->get_parent();
  if (parent != 0)
    EmitPath(parent, out);

  out << "\t" << node->GetMethodFullName();
}

void CCTree::EmitTreeMapTM3(CCNode* node, std::ofstream & out)
{
  int abytes = node->get_total_allocated_objects();
  int dbytes = node->get_total_dead_bytes();

  if (abytes > 0 || dbytes > 0) {
    const CCNode::CCNodeVector & children = node->get_children();

    // Compute total size of children
    int child_total_alloc = 0;
    int child_total_dead = 0;
    int num_children = 0;
    for (CCNode::CCNodeVector::const_iterator p = children.begin();
         p != children.end();
         p++)
    {
      CCNode * child = (*p);
      int child_alloc = child->get_total_allocated_objects();
      int child_dead =  child->get_total_dead_bytes();
      child_total_alloc += child_alloc;
      child_total_dead  += child_dead;
      if (child_alloc > 0 || child_dead > 0)
        num_children++;
    }

    // Output entry for this node
    out << (abytes+dbytes) << "\t";
    out << node->get_calls() << "\t";
    out << "0" << "\t";
    out << node->get_thread_id() << "\t";
    out << node->get_first_call() << "\t";
    EmitPath(node, out);
    out << std::endl;

    // Output synthetic node representing the this node's contribution
    // (if necessary)
    if (num_children > 0) {
      int alloc_diff = abytes - child_total_alloc;
      int dead_diff = dbytes - child_total_dead;
      if (alloc_diff > 0) {
        out << alloc_diff << "\t";
        out << node->get_calls() << "\t";
        out << "0" << "\t";
        out << node->get_thread_id() << "\t";
        out << node->get_first_call() << "\t";
        EmitPath(node, out);
        out << "\talloc";
        out << std::endl;
      }
      if (dead_diff > 0) {
        out << dead_diff << "\t";
        out << node->get_calls() << "\t";
        out << "0" << "\t";
        out << node->get_thread_id() << "\t";
        out << node->get_first_call() << "\t";
        EmitPath(node, out);
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
      EmitTreeMapTM3(child, out);
    }
  }
}

void CCTree::EmitTreeMapTM3(std::ofstream & out)
{
  out << "Bytes\tCalls\tLifetime\tThread\tFirst call" << std::endl;
  out << "INTEGER\tINTEGER\tINTEGER\tINTEGER\tINTEGER" << std::endl;

  EmitTreeMapTM3(root, out);
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

  EmitTreeML(root, out, 1);

  out << "</tree>" << std::endl;
}

void CCTree::EmitTreeML(CCNode * node, std::ofstream & out, int depth)
{
  int abytes = node->get_total_allocated_bytes();
  int dbytes = node->get_total_dead_bytes();

  if (abytes + dbytes > 0) {
    const CCNode::CCNodeVector & children = node->get_children();
    bool is_leaf = (children.size() == 0);

    if (is_leaf) {
      // -- Leaf
      out << "<leaf>" << std::endl;
    } else {
      out << "<branch>" << std::endl;
    }

    Method * meth = Method::getMethod(node->get_method_id());
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
        << (node->get_allocated_objects() + node->get_dead_objects()) << "\"/>"
        << std::endl;
    out << "<attribute name=\"objalloc\" value=\"" << node->get_allocated_objects()
        << "\"/>" << std::endl;
    out << "<attribute name=\"objdead\" value=\"" << node->get_dead_objects()
        << "\"/>" << std::endl;
    out << "<attribute name=\"thread\" value=\""
        << threadIdNumbering[node->get_thread_id()] << "\"/>" << std::endl;

    for (CCNode::CCNodeVector::const_iterator p = children.begin();
         p != children.end();
         p++)
    {
      CCNode * child = (*p);
      EmitTreeML(child, out, depth+1);
    }

    if (is_leaf) {
      // -- Leaf
      out << "</leaf>" << std::endl;
    } else {
      out << "</branch>" << std::endl;
    }
  }
}

void CCTree::EmitTreeJSON(std::ofstream & out)
{
  EmitTreeJSON(root, out, 0);
}

void CCTree::EmitTreeJSON(CCNode* node, std::ofstream & out, int depth)
{
  int abytes = node->get_total_allocated_bytes();
  int dbytes = node->get_total_dead_bytes();

  if (abytes + dbytes > 0) {
    std::string space(depth*2, ' ');
    out << space << "{" << std::endl;

    Method * meth = Method::getMethod(node->get_method_id());
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

    const CCNode::CCNodeVector & children = node->get_children();
    if (children.size() == 0) {
      out << space << " \"size\": " << abytes << std::endl;
    } else {
      out << space << " \"children\": [" << abytes << std::endl;
      for (CCNode::CCNodeVector::const_iterator p = children.begin();
           p != children.end();
           p++)
      {
        CCNode * child = (*p);
        EmitTreeJSON(child, out, depth+1);
      }
      out << space << " ]" << std::endl;
    }
    out << space << "}" << std::endl;
  }
}
}  // namespace et_simulator
