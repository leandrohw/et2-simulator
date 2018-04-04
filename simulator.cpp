
#include "cctree.h"
#include "heap.h"

int debug = 0;

// -- Global counters

long total_alloc_size = 0;
long record_count = 0;
long no_alloc = 0;

// -- Multi-threaded stack
typedef map<int, CCNode *> StackMap;
StackMap theStack;

//    Map from thread objects to the context in which start() was called
StackMap threadStarts;
int thread_start_method_id = 0;
int thread_number = 0;
map<int, int> threadIdNumbering;

void read_trace_file(ifstream & in, CCNode * root)
{
  HeapObject * heapObject;
  HeapObject * targetObject;

  // CCNode * current_node = root;
  theStack[0] = root;
  threadStarts[0] = root;
  CCNode * node;
  int depth = 0;
  long time = 0;
  bool in_death_records = false;
  long last_epoch_time = 0;
  long last_thread_id = 0;

  while ( ! in.eof()) {
    char kind;
    string line;
    int object_id;
    long dtime;
    int size;
    string type;
    int method_id;
    string class_name;
    string method_name;
    string signature;
    int old_target;
    int new_target;
    int thread_id;
    int root_id;

    Method * method;

    in >> kind;
    if (in.fail())
      break;
    // in >> hex >> time;

    /*
    if (in_death_records && (kind != 'D')) {
      in_death_records = false;
      last_epoch_time = time;
    }
    */

    switch (kind) {
    case 'A':
      {
        in >> hex >> object_id;
        in >> hex >> size;
        in >> type;
        in >> hex >> thread_id;
        
        CCNode * curContext = 0;
        
        if (thread_id == object_id) {
          // -- Spawning a new thread -- make it a child of the root
          curContext = root; // theStack[last_thread_id];        
          CCNode * newContext = curContext->demand_child(method_id, thread_id, time);
          newContext->incCalls();
          theStack[thread_id] = newContext;
          if (debug > 0)
            cout << "Spawn thread 0x" << hex << thread_id << dec << endl;
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
          cout << "Allocate 0x" << hex << object_id << " size 0x" << size << dec << " at time " << time << endl;
          if (debug > 1) curContext->printStack();
	}	

	last_thread_id = thread_id;
      }
      break;
    case 'D':
      {
        in >> hex >> object_id;
        
        heapObject = HeapObject::DemandHeapObject(object_id);
        heapObject->setDead(time);
        (HeapObject::Find(heapObject))->incNumDead();
        
        CCNode * curContext = theStack[last_thread_id];
        
        curContext->incDeadBytes(heapObject->getSize());
        curContext->incDeadObjects();
        heapObject->setDeathCC(curContext);
      }
      break;
    case 'U':
      in >> hex >> old_target;
      in >> hex >> object_id;
      in >> hex >> new_target;
      in >> hex >> thread_id;
      if (object_id != 0 and new_target != 0) {
	heapObject = HeapObject::DemandHeapObject(object_id);
	targetObject = HeapObject::DemandHeapObject(new_target);
	HeapObject * sroot = HeapObject::Find(heapObject);
	HeapObject * troot = HeapObject::Find(targetObject);
	if ((targetObject->getType() != "[C") and
	    (targetObject->getType() != "UNKNOWN") and
	    (troot->getId() != 0x19a)) {
	  if (debug > 0) {
	    cout << "Pointer from 0x" << hex << object_id << " " << heapObject->getType()
		 << " to 0x" << new_target << dec << " " << targetObject->getType() << " at time " << time << endl;
	    if (debug > 1) {
	      CCNode * curContext = theStack[last_thread_id];
	      curContext->printStack();
	    }
	  }

	  // HeapObject::Union(heapObject, targetObject);
	  heapObject->setPointsTo(targetObject);
	}
      }
      break;
    case 'M':
      {
        in >> hex >> method_id;
        in >> hex >> object_id;
        in >> hex >> thread_id;
        
        CCNode * curContext = theStack[thread_id];
        
        bool new_thread = false;
        if (curContext == 0) {
          // -- Spawning a new thread -- look up the place where the thread was started
          //    Relies on the fact that the thread_id is the same as the object_id of
          //    the Thread object instance.
          new_thread = true;
          curContext = threadStarts[thread_id];
          if (curContext) {
	    cout << "Spawn thread 0x" << hex << thread_id << dec << " -- started at " << curContext->getMethodFullName() << endl;
            cout << "   in context" << endl;
            curContext->printStack();
          } else {
            cout << "Problem: no threadStart for thread id 0x" << hex << thread_id << dec << endl;
            curContext = root;
          }
        }
        
        time++;
        depth++;
        
        curContext = curContext->demand_child(method_id, thread_id, time);
        curContext->incCalls();
        theStack[thread_id] = curContext;
        
        if (debug > 0 or new_thread) {
          cout << "Enter " << curContext->getMethodFullName() << " 0x" << hex << method_id << " thread 0x" << thread_id << dec << " at time " << time << endl;
          if (debug > 1) curContext->printStack();
	}
	
	if (method_id == thread_start_method_id) {
	  // -- Found a new thread start
	  threadStarts[object_id] = curContext;
	  thread_number++;
	  threadIdNumbering[object_id] = thread_number;
	  if (true) {
	    cout << "Found Thread.start at " << endl;
	    curContext->printStack();
	  }
	}
        
        last_thread_id = thread_id;
      }
      break;
    case 'E':
      {
        in >> hex >> method_id;
        in >> hex >> object_id;
        in >> hex >> thread_id;
        
        CCNode * curContext = theStack[thread_id];
	if (theStack.find(thread_id) == theStack.end()) {
	  cout << "Never seen thread 0x" << hex << thread_id << endl;
	}

	if (curContext == 0) {
	  Method * meth = Method::getMethod(method_id);
	  cout << "ERROR: Thread 0x" << hex << thread_id << " returning from method " << meth->getMethodName() << " in class " << meth->getClassName() << endl;
	  break;
	}

        if (debug > 0) {
          cout << "Exit  " << curContext->getMethodFullName() << " 0x" << hex << method_id << " thread 0x" << thread_id << dec << " at time " << time << endl;
          if (debug > 1) curContext->printStack();
	}	

        time++;
        
        CCNode * returning = curContext;
        int cur_id = returning->getMethodId();
        int orig_depth = depth;
        while (returning and returning->getMethodId() != method_id) {
          returning = returning->getParent();
          depth--;
        }
        
        if (returning == 0) {
          cout << "THIS IS BAD: looking for " 
               << hex << "0x" << method_id << " but found 0x" 
               << cur_id << dec << endl;
          returning->printStack();
          // current_node unchanged
          depth = orig_depth;
        } else {
          // cout << "Return " << current_node->getMethodFullName() << "(" << hex << current_node->getMethodId() << dec << ")" << endl;
          returning->setLastCall(time);
          theStack[thread_id] = returning->getParent();
          depth--;
        }
        
        last_thread_id = thread_id;
      }
      break;

    case 'R':
      // -- Throw out roots for now
      in >> hex >> root_id;
      in >> hex >> thread_id;
      break;

    default:
      cout << "UNKNOWN" << endl;
      break;
    }

    getline(in, line);

    record_count++;
    if (record_count % 1000000 == 0) {
      cerr << "At " << record_count << endl;
    }
  }
}

void read_name_file(ifstream & name_file)
{
  while ( ! name_file.eof()) {
    char kind;
    int method_id;
    string class_name;
    string method_name;
    string signature;
    int class_id;
    int super_id;
    char inher;
    char flag;
    int field_id;
    string field_name;
    string type;
    
    name_file >> kind;

    switch (kind) {
    case 'N':
      {
	name_file >> hex >> method_id;
	name_file >> class_name;
	name_file >> method_name;
	name_file >> signature;
	Method * method = new Method(method_id, class_name, method_name, signature);
	if (class_name == "java/lang/Thread" and method_name == "start") {
	  thread_start_method_id = method_id;
	}
      }    
      break;

    case 'C':
      name_file >> hex >> class_id;
      name_file >> class_name;
      name_file >> hex >> super_id;
      name_file >> inher;
     break;

    case 'F':
      name_file >> flag;
      name_file >> hex >> field_id;
      name_file >> field_name;
      name_file >> hex >> class_id;
      name_file >> class_name;
      name_file >> type;
      break;

    case 'I':
      name_file >> hex >> class_id;
      name_file >> class_name;
      break;

    case 'E':
      name_file >> hex >> class_id;
      name_file >> class_name;
      break;
    }
  }
}

int main(int argc, char * argv[])
{
  if (argc < 3) {
    cout << "Usage: cctree <tracefile> <namesfile>" << endl;
    exit(0);
  }

  CCNode * root = new CCNode(0, 0, 0, 0);

  cout << "Read names..." << endl;

  ifstream name_file;
  name_file.open(argv[2]);
  if (name_file.fail()) {
    cout << "Failed to open name file " << argv[2] << endl;
    exit(0);
  }

  read_name_file(name_file);

  cout << "Read events..." << endl;

  ifstream in;
  in.open(argv[1]);
  if (in.fail()) {
    cout << "Failed to open name file " << argv[1] << endl;
    exit(0);
  }

  read_trace_file(in, root);
}

