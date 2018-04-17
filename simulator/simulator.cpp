#include "simulator.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/strings/numbers.h"

bool Simulator::parse_object_allocation(std::vector<std::string> trace) {
  if(trace.size() != 5) {
    return false;
  }

  type = trace[3];

  return (absl::SimpleAtoi(trace[1], &object_id) &&
          absl::SimpleAtoi(trace[2], &size) &&
          absl::SimpleAtoi(trace[4], &thread_id));

}

bool Simulator::parse_object_update(std::vector<std::string> trace) {
  if(trace.size() != 5) {
    return false;
  }

  return (absl::SimpleAtoi(trace[1], &old_target) &&
          absl::SimpleAtoi(trace[2], &object_id) &&
          absl::SimpleAtoi(trace[3], &new_target) &&
          absl::SimpleAtoi(trace[4], &thread_id));
}

bool Simulator::parse_method(std::vector<std::string> trace) {
  if(trace.size() != 4) {
    return false;
  }

  return (absl::SimpleAtoi(trace[1], &method_id)&&
          absl::SimpleAtoi(trace[2], &object_id) &&
          absl::SimpleAtoi(trace[3], &thread_id));
}


bool Simulator::execute(std::string line) {
    std::vector<std::string> trace = absl::StrSplit(line, ' ');

    if(trace.size() < 1 || trace[0].length() != 1) {
      return false;
    }

    kind = trace[0][0];

    switch (kind) {
      case 'A':
        if(!parse_object_allocation(trace)) {
           // Log bad allocation entry
          return false;
        }

        tree->handle_object_allocation(object_id, size, type,
                                       thread_id, method_id);
        return true;
      case 'D':
        //in >> std::hex >> object_id;
        //tree->handle_object_death(object_id);
        return true;
      case 'U':
        if(!parse_object_update(trace)) {
          // Log bad update entry
          return false;
        }
        tree->handle_object_update(old_target, object_id,
                                   new_target, thread_id);
        return true;
      case 'M':
        if(!parse_method(trace)) {
          // Log bad method entry name
          return false;
        }
        tree->handle_method_entry(method_id, object_id, thread_id);
        return true;
      case 'E':
        if(! parse_method(trace)) {
          // Log bad entry name
          return false;
        }
        tree->handle_method_exit(method_id, object_id, thread_id);
        return true;
      case 'R':
        // -- Throw out roots for now
        return true;
    }

    std::cout << "UNKNOWN" << std::endl;
    return false;
}

void Simulator::read_trace_file() {
  std::string line;
  int64_t record_count = 0;

  std::ifstream in;
  in.open(tracefile);

  if (in.fail()) {
    std::cout << "Failed to open name file " << tracefile << std::endl;
    exit(EXIT_FAILURE);
  }


  while (!in.eof()) {
    if (record_count % 1000000 == 0) {
      std::cerr << "At " << record_count << std::endl;
    }

    getline(in, line);

    if (in.fail())
      break;

    if(!execute(line)) {
      exit(EXIT_FAILURE);
    }

    record_count++;
  }
}

// void Simulator::read_name_file()
// {
//     char kind;
//     int method_id;
//     std::string class_name;
//     std::string method_name;
//     std::string signature;
//     int class_id;
//     int super_id;
//     char inher;
//     char flag;
//     int field_id;
//     std::string field_name;
//     std::string type;
// std::ifstream name_file;
// name_file.open(namesfile);
//
// if (name_file.fail()) {
//   std::cout << "Failed to open name file " << namesfile << std::endl;
//   exit(EXIT_FAILURE);
// }
//
//   while (!name_file.eof()) {

//
//     name_file >> kind;
//
//     switch (kind) {
//       case 'N':
//         {
//           name_file >> std::hex >> method_id;
//           name_file >> class_name;
//           name_file >> method_name;
//           name_file >> signature;
//           Method * method = new Method(method_id, class_name,
//                                        method_name, signature);
//           if (class_name == "java/lang/Thread" && method_name == "start") {
//             thread_start_method_id = method_id;
//           }
//         }
//         break;
//
//       case 'C':
//         name_file >> std::hex >> class_id;
//         name_file >> class_name;
//         name_file >> std::hex >> super_id;
//         name_file >> inher;
//         break;
//
//       case 'F':
//         name_file >> flag;
//         name_file >> std::hex >> field_id;
//         name_file >> field_name;
//         name_file >> std::hex >> class_id;
//         name_file >> class_name;
//         name_file >> type;
//         break;
//
//       case 'I':
//         name_file >> std::hex >> class_id;
//         name_file >> class_name;
//         break;
//
//       case 'E':
//         name_file >> std::hex >> class_id;
//         name_file >> class_name;
//         break;
//     }
//   }
// }

void Simulator::simulate() {
  read_trace_file();
  report();
}

void Simulator::report() {
  // TODO(leandrohw): decide how to output data

}
