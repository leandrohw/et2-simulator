#include "cctree.h"
#include "method.h"
int debug = 0;


void CCTree::handle_object_allocation(int object_id, int size, std::string type, int thread_id, int method_id) {

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
    if (debug > 0)
      std::cout << "Spawn thread 0x" << std::hex << thread_id << std::dec << std::endl;
  }

  if (theStack[thread_id] == 0) {
    theStack[thread_id] = root;
  }

  curContext = theStack[thread_id];

  curContext->incAllocBytes(size);
  curContext->incAllocObjects();
  heapObject = HeapObject::DemandHeapObject(object_id);
  heapObject->setAlloc(time, size, type);
  heapObject->setAllocCC(curContext);

  if (debug > 0) {
    std::cout << "Allocate 0x" << std::hex << object_id << " size 0x" << size
         << std::dec << " at time " << time << std::endl;

    //if (debug > 1) curContext->printStack();
  }
  last_thread_id = thread_id;
}

void CCTree::handle_object_death(int object_id) {
  heapObject = HeapObject::DemandHeapObject(object_id);
  heapObject->setDead(time);
  (HeapObject::Find(heapObject))->incNumDead();

  CCNode * curContext = theStack[last_thread_id];

  curContext->incDeadBytes(heapObject->getSize());
  curContext->incDeadObjects();
  heapObject->setDeathCC(curContext);
}

void CCTree::handle_object_update(int old_target, int object_id, int new_target, int target_id) {
  if (object_id != 0 && new_target != 0) {
    heapObject = HeapObject::DemandHeapObject(object_id);
    targetObject = HeapObject::DemandHeapObject(new_target);
    HeapObject * sroot = HeapObject::Find(heapObject);
    HeapObject * troot = HeapObject::Find(targetObject);
    if (targetObject->getType() != "[C" &&
        targetObject->getType() != "UNKNOWN" &&
        troot->getId() != 0x19a) {
      if (debug > 0) {
        std::cout << "Pointer from 0x" << std::hex << object_id << " "
             << heapObject->getType() << " to 0x" << new_target
                  << std::dec << " " << targetObject->getType() << " at time "
             << time << std::endl;
        // if (debug > 1) {
        //   CCNode * curContext = theStack[last_thread_id];
        //   curContext->printStack();
        // }
      }

      // HeapObject::Union(heapObject, targetObject);
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
      std::cout << "Spawn thread 0x" << std::hex << thread_id << std::dec
           << " -- started at " << curContext->getMethodFullName()
           << std::endl;
      std::cout << "   in context" << std::endl;
      //curContext->printStack();
    } else {
      std::cout << "Problem: no threadStart for thread id 0x" << std::hex
                << thread_id << std::dec << std::endl;
      curContext = root;
    }
  }

  time++;
  depth++;

  curContext = curContext->demand_child(method_id, thread_id, time);
  curContext->incCalls();
  theStack[thread_id] = curContext;

  if (debug > 0 || new_thread) {
    std::cout << "Enter " << curContext->getMethodFullName() << " 0x"
              << std::hex << method_id << " thread 0x" << thread_id << std::dec
         << " at time " << time << std::endl;
    //if (debug > 1) curContext->printStack();
  }

  if (method_id == thread_start_method_id) {
    // -- Found a new thread start
    threadStarts[object_id] = curContext;
    thread_number++;
    threadIdNumbering[object_id] = thread_number;
    if (true) {
      std::cout << "Found Thread.start at " << std::endl;
      //curContext->printStack();
    }
  }

  last_thread_id = thread_id;
}

void CCTree::handle_method_exit(int method_id, int object_id, int thread_id) {
  CCNode * curContext = theStack[thread_id];
  if (theStack.find(thread_id) == theStack.end()) {
    std::cout << "Never seen thread 0x" << std::hex << thread_id << std::endl;
  }

  if (curContext == 0) {
    Method * meth = Method::getMethod(method_id);
    std::cout << "ERROR: Thread 0x" << std::hex << thread_id
         << " returning from method " << meth->getMethodName()
         << " in class " << meth->getClassName() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (debug > 0) {
    std::cout << "Exit  " << curContext->getMethodFullName() << " 0x"
              << std::hex << method_id << " thread 0x" << thread_id << std::dec
         << " at time " << time << std::endl;
    //if (debug > 1) curContext->printStack();
  }

  time++;

  CCNode * returning = curContext;
  int cur_id = returning->getMethodId();
  int orig_depth = depth;
  while (returning || returning->getMethodId() != method_id) {
    returning = returning->getParent();
    depth--;
  }

  if (returning == 0) {
    std::cout << "THIS IS BAD: looking for "
         << std::hex << "0x" << method_id << " but found 0x"
              << cur_id << std::dec << std::endl;
    //returning->printStack();
    // current_node unchanged
    depth = orig_depth;
  } else {
    returning->setLastCall(time);
    theStack[thread_id] = returning->getParent();
    depth--;
  }

  last_thread_id = thread_id;
}
