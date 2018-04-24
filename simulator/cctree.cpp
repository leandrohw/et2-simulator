#include "simulator/cctree.h"
#include "simulator/method.h"
#include "glog/logging.h"

namespace et_simulator {
int debug = 0;
void CCTree::HandleObjectAllocation(int object_id, int size,
                                    std::string type, int thread_id) {
  CCNode * current_context = 0;

  if (thread_id == object_id) {
    // Spawning a new thread
    // Make it a child of the root_
    current_context = root_;
    CCNode * new_context = current_context->DemandChild(last_method_id_,
                                                  thread_id,
                                                  time);
    new_context->IncrementCalls();
    stack__[thread_id] = new_context;
    LOG(INFO) << "Spawn thread 0x" << std::hex << thread_id
              << std::dec;
  }

  if (stack__[thread_id] == 0) {
    stack__[thread_id] = root_;
  }

  current_context = stack__[thread_id];

  current_context->IncrementAllocatedBytes(size);
  current_context->IncrementAllocatedObjects();

  HeapObject * heap_object = HeapObject::DemandHeapObject(object_id);
  heap_object->MarkAllocated(time, size, type);
  heap_object->MarkAllocatedCC(current_context);

  LOG(INFO) <<  "Allocate 0x" << std::hex << object_id << " size 0x" << size << std::dec << " at time " << time;
  LogStack(current_context);

  last_thread_id_ = thread_id;
}

void CCTree::HandleObjectDeath(int object_id) {
  HeapObject * heap_object = HeapObject::DemandHeapObject(object_id);
  heap_object->MarkDead(time);
  (HeapObject::Find(heap_object))->IncrementNumberOfDeadObjects();

  CCNode * current_context = stack__[last_thread_id_];

  current_context->IncrementDeadBytes(heap_object->get_size());
  current_context->IncrementDeadObjects();
  heap_object->setDeathCC(current_context);
}

void CCTree::HandleObjectUpdate(int old_target, int object_id,
                                int new_target, int target_id) {
  if (object_id != 0 && new_target != 0) {
    HeapObject * heap_object = HeapObject::DemandHeapObject(object_id);
    HeapObject * target_object = HeapObject::DemandHeapObject(new_target);
    HeapObject * target_root_ = HeapObject::Find(target_object);
    if (target_object->get_type() != "[C" &&
        target_object->get_type() != "UNKNOWN" &&
        target_root_->get_id() != 0x19a) {

      LOG(INFO) << "Pointer from 0x" << std::hex << object_id << " "
                << heap_object->get_type() << " to 0x" << new_target
                << std::dec << " " << target_object->get_type() << " at time "
                << time;

      CCNode * current_context = stack__[last_thread_id_];
      LogStack(current_context);

      heap_object->set_points_to(target_object);
    }
  }
}

void CCTree::HandleMethodEntry(int method_id, int object_id, int thread_id) {
  CCNode * current_context = stack__[thread_id];
  last_method_id_ = method_id;
  if (current_context == 0) {
    // Spawning a new thread -- look up where the thread started.
    // Relies on the fact that the thread_id is the same as the
    // object_id of the Thread object instance.
    current_context = thread_starts[thread_id];
    if (current_context) {
      LOG(INFO) << "Spawn thread 0x" << std::hex << thread_id << std::dec
                << " -- started at " << current_context->GetMethodFullName()
                << " in context";
      LogStack(current_context);
    } else {
      current_context = root_;
      LOG(INFO) << "Problem: no threadStart for thread id 0x" << std::hex
                << thread_id << std::dec;
    }
  }

  time++;
  depth_++;

  current_context = current_context->DemandChild(method_id, thread_id, time);
  current_context->IncrementCalls();
  stack__[thread_id] = current_context;

  LOG(INFO) << "Enter " << current_context->GetMethodFullName() << " 0x"
            << std::hex << method_id << " thread 0x" << thread_id << std::dec
            << " at time " << time;
  LogStack(current_context);

  if (method_id == thread_start_method_id_) {
    // Found a new thread start
    thread_starts[object_id] = current_context;
    thread_number_++;
    thread_id_map_[object_id] = thread_number_;
    LOG(INFO) << "Found Thread.start at ";
    LogStack(current_context);
  }

  last_thread_id_ = thread_id;
}

void CCTree::HandleMethodExit(int method_id, int object_id, int thread_id) {
  CCNode * current_context = stack__[thread_id];
  last_method_id_ = method_id;

  if (stack__.find(thread_id) == stack__.end()) {
    LOG(INFO) << "Never seen thread 0x" << std::hex << thread_id;
  }

  if (current_context == 0) {
    Method * method = Method::getMethod(method_id);
    LOG(FATAL) << "ERROR: Thread 0x" << std::hex << thread_id
               << " returning from method " << method->getMethodName()
               << " in class " << method->getClassName();
  }

  LOG(INFO) << "Exit  " << current_context->GetMethodFullName() << " 0x"
            << std::hex << method_id << " thread 0x" << thread_id << std::dec
            << " at time " << time;
  LogStack(current_context);

  time++;
  CCNode * returning = current_context;
  int cur_id = returning->get_method_id();
  int orig_depth_ = depth_;
  while (returning && returning->get_method_id() != method_id) {
    returning = returning->get_parent();
    depth_--;
  }

  if (returning == 0) {
    LOG(INFO) << "THIS IS BAD: looking for "
              << std::hex << "0x" << method_id << " but found 0x"
              << cur_id << std::dec;
    LogStack(returning);
    // current_node unchanged
    depth_ = orig_depth_;
  } else {
    returning->set_last_call(time);
    stack__[thread_id] = returning->get_parent();
    depth_--;
  }
  last_thread_id_ = thread_id;
}


void CCTree::PrintTree(CCNode* node, int depth_)
{
  if (node->get_total_allocated_objects() == 0 &&
      node->get_total_allocated_bytes() == 0)
    return;

  for (int i = 0; i < depth_; i++)
    std::cout << ".";

  if (depth_ > 0)
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
    PrintTree(child, depth_+1);
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

  EmitTreeMapTM3(root_, out);
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

  EmitTreeML(root_, out, 1);

  out << "</tree>" << std::endl;
}

void CCTree::EmitTreeML(CCNode * node, std::ofstream & out, int depth_)
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

    Method * method = Method::getMethod(node->get_method_id());
    std::string nm;
    std::string classnm;
    if (method) {
      nm = method->getMethodName();
      classnm = method->getClassName();
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
        << thread_id_map_[node->get_thread_id()] << "\"/>" << std::endl;

    for (CCNode::CCNodeVector::const_iterator p = children.begin();
         p != children.end();
         p++)
    {
      CCNode * child = (*p);
      EmitTreeML(child, out, depth_+1);
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
  EmitTreeJSON(root_, out, 0);
}

void CCTree::EmitTreeJSON(CCNode* node, std::ofstream & out, int depth_)
{
  int abytes = node->get_total_allocated_bytes();
  int dbytes = node->get_total_dead_bytes();

  if (abytes + dbytes > 0) {
    std::string space(depth_*2, ' ');
    out << space << "{" << std::endl;

    Method * method = Method::getMethod(node->get_method_id());
    std::string nm;
    std::string classnm;
    if (method) {
      nm = method->getMethodName();
      classnm = method->getClassName();
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
        EmitTreeJSON(child, out, depth_+1);
      }
      out << space << " ]" << std::endl;
    }
    out << space << "}" << std::endl;
  }
}
}  // namespace et_simulator
