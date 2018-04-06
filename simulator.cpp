#include "simulator.h"


int debug = 0;

void Simulator::handle_object_allocation() {
  /*
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
      std::cout << "Spawn thread 0x" << std::hex << thread_id << dec << std::endl;
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
         << dec << " at time " << time << std::endl;

    if (debug > 1) curContext->printStack();
  }
  last_thread_id = thread_id;
  */
}

void Simulator::handle_object_death() {
  // heapObject = HeapObject::DemandHeapObject(object_id);
  // heapObject->setDead(time);
  // (HeapObject::Find(heapObject))->incNumDead();

  // CCNode * curContext = theStack[last_thread_id];

  // curContext->incDeadBytes(heapObject->getSize());
  // curContext->incDeadObjects();
  // heapObject->setDeathCC(curContext);
}

void Simulator::handle_object_update() {
//   if (object_id != 0 && new_target != 0) {
//     heapObject = HeapObject::DemandHeapObject(object_id);
//     targetObject = HeapObject::DemandHeapObject(new_target);
//     HeapObject * sroot = HeapObject::Find(heapObject);
//     HeapObject * troot = HeapObject::Find(targetObject);
//     if (targetObject->getType() != "[C" &&
//         targetObject->getType() != "UNKNOWN" &&
//         troot->getId() != 0x19a) {
//       if (debug > 0) {
//         std::cout << "Pointer from 0x" << std::hex << object_id << " "
//              << heapObject->getType() << " to 0x" << new_target
//              << dec << " " << targetObject->getType() << " at time "
//              << time << std::endl;
//         if (debug > 1) {
//           CCNode * curContext = theStack[last_thread_id];
//           curContext->printStack();
//         }
//       }

//       // HeapObject::Union(heapObject, targetObject);
//       heapObject->setPointsTo(targetObject);
//     }
//   }
}

void Simulator::handle_method_entry() {
  // CCNode * curContext = theStack[thread_id];
  // bool new_thread = false;
  // if (curContext == 0) {
  //   // Spawning a new thread -- look up where the thread started.
  //   // Relies on the fact that the thread_id is the same as the
  //   // object_id of the Thread object instance.
  //   new_thread = true;
  //   curContext = threadStarts[thread_id];
  //   if (curContext) {
  //     std::cout << "Spawn thread 0x" << std::hex << thread_id << dec
  //          << " -- started at " << curContext->getMethodFullName()
  //          << std::endl;
  //     std::cout << "   in context" << std::endl;
  //     curContext->printStack();
  //   } else {
  //     std::cout << "Problem: no threadStart for thread id 0x" << std::hex
  //          << thread_id << dec << std::endl;
  //     curContext = root;
  //   }
  // }

  // time++;
  // depth++;

  // curContext = curContext->demand_child(method_id, thread_id, time);
  // curContext->incCalls();
  // theStack[thread_id] = curContext;

  // if (debug > 0 || new_thread) {
  //   std::cout << "Enter " << curContext->getMethodFullName() << " 0x"
  //        << std::hex << method_id << " thread 0x" << thread_id << dec
  //        << " at time " << time << std::endl;
  //   if (debug > 1) curContext->printStack();
  // }

  // if (method_id == thread_start_method_id) {
  //   // -- Found a new thread start
  //   threadStarts[object_id] = curContext;
  //   thread_number++;
  //   threadIdNumbering[object_id] = thread_number;
  //   if (true) {
  //     std::cout << "Found Thread.start at " << std::endl;
  //     curContext->printStack();
  //   }
  // }

  // last_thread_id = thread_id;
}

void Simulator::handle_method_exit() {
  // CCNode * curContext = theStack[thread_id];
  // if (theStack.find(thread_id) == theStack.end()) {
  //   std::cout << "Never seen thread 0x" << std::hex << thread_id << std::endl;
  // }

  // if (curContext == 0) {
  //   Method * meth = Method::getMethod(method_id);
  //   std::cout << "ERROR: Thread 0x" << std::hex << thread_id
  //        << " returning from method " << meth->getMethodName()
  //        << " in class " << meth->getClassName() << std::endl;
  //   break;
  // }

  // if (debug > 0) {
  //   std::cout << "Exit  " << curContext->getMethodFullName() << " 0x"
  //        << std::hex << method_id << " thread 0x" << thread_id << dec
  //        << " at time " << time << std::endl;
  //   if (debug > 1) curContext->printStack();
  // }

  // time++;

  // CCNode * returning = curContext;
  // int cur_id = returning->getMethodId();
  // int orig_depth = depth;
  // while (returning || returning->getMethodId() != method_id) {
  //   returning = returning->getParent();
  //   depth--;
  // }

  // if (returning == 0) {
  //   std::cout << "THIS IS BAD: looking for "
  //        << std::hex << "0x" << method_id << " but found 0x"
  //        << cur_id << dec << std::endl;
  //   returning->printStack();
  //   // current_node unchanged
  //   depth = orig_depth;
  // } else {
  //   returning->setLastCall(time);
  //   theStack[thread_id] = returning->getParent();
  //   depth--;
  // }

  // last_thread_id = thread_id;
}

void Simulator::read_trace_file(std::ifstream & in, CCNode * root) {
  HeapObject * heapObject;
  HeapObject * targetObject;

  int depth = 0;
  int64_t time = 0;
  bool in_death_records = false;
  int64_t last_thread_id = 0;
  int64_t record = 0;

  while (!in.eof()) {
    char kind;
    std::string line;
    int object_id;
    std::string type;
    int method_id;
    int old_target;
    int new_target;
    int thread_id;
    int root_id;
    int size;

    in >> kind;
    if (in.fail())
      break;

    switch (kind) {
      case 'A':
          in >> std::hex >> object_id;
          in >> std::hex >> size;
          in >> type;
          in >> std::hex >> thread_id;
          //Simulator::handle_object_allocation(object_id, size, type, thread_id);
        break;
      case 'D':
          in >> std::hex >> object_id;
          //Simulator::handle_object_death(object_id);
        break;
      case 'U':
        in >> std::hex >> old_target;
        in >> std::hex >> object_id;
        in >> std::hex >> new_target;
        in >> std::hex >> thread_id;
        // Simulator::handle_object_update(old_target, object_id,
        //                                 new_target, thread_id);
        break;
      case 'M':
          in >> std::hex >> method_id;
          in >> std::hex >> object_id;
          in >> std::hex >> thread_id;
          // Simulator::handle_method_entry(method_id, object_id, thread_id);

        break;
      case 'E':
          in >> std::hex >> method_id;
          in >> std::hex >> object_id;
          in >> std::hex >> thread_id;
          // Simulator::handle_method_exit(method_id, object_id, thread_id);
        break;

      case 'R':
        // -- Throw out roots for now
        in >> std::hex >> root_id;
        in >> std::hex >> thread_id;
        break;

      default:
        std::cout << "UNKNOWN" << std::endl;
        break;
    }

    getline(in, line);

    record_count++;
    if (record_count % 1000000 == 0) {
      std::cerr << "At " << record_count << std::endl;
    }
  }
}

void Simulator::read_name_file(std::ifstream & name_file)
{
  while (!name_file.eof()) {
    char kind;
    int method_id;
    std::string class_name;
    std::string method_name;
    std::string signature;
    int class_id;
    int super_id;
    char inher;
    char flag;
    int field_id;
    std::string field_name;
    std::string type;

    name_file >> kind;

    switch (kind) {
      case 'N':
        {
          name_file >> std::hex >> method_id;
          name_file >> class_name;
          name_file >> method_name;
          name_file >> signature;
          Method * method = new Method(method_id, class_name,
                                       method_name, signature);
          if (class_name == "java/lang/Thread" && method_name == "start") {
            thread_start_method_id = method_id;
          }
        }
        break;

      case 'C':
        name_file >> std::hex >> class_id;
        name_file >> class_name;
        name_file >> std::hex >> super_id;
        name_file >> inher;
        break;

      case 'F':
        name_file >> flag;
        name_file >> std::hex >> field_id;
        name_file >> field_name;
        name_file >> std::hex >> class_id;
        name_file >> class_name;
        name_file >> type;
        break;

      case 'I':
        name_file >> std::hex >> class_id;
        name_file >> class_name;
        break;

      case 'E':
        name_file >> std::hex >> class_id;
        name_file >> class_name;
        break;
    }
  }
}

void Simulator::simulate() {
  last_thread_id = 0;
  //theStack[0] = root;
  //threadStarts[0] = root;
  //Simulator::read_trace_file();
}
