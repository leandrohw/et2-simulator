#include "simulator/simulator.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/strings/numbers.h"
#include "glog/logging.h"

namespace et_simulator {
bool Simulator::ParseObjectAllocation(std::vector<std::string> trace) {
  if (trace.size() != 5) {
    return false;
  }

  std::string type = trace[3];
  int object_id;
  int size;
  int thread_id;

  if (absl::SimpleAtoi(trace[1], &object_id) &&
     absl::SimpleAtoi(trace[2], &size) &&
     absl::SimpleAtoi(trace[4], &thread_id)) {
    tree_->HandleObjectAllocation(object_id, size,
                                   type, thread_id);
    return true;
  }
  return false;
}

bool Simulator::ParseObjectUpdate(std::vector<std::string> trace) {
  if (trace.size() != 5) {
    return false;
  }

  int old_target;
  int object_id;
  int new_target;
  int thread_id;

  if (absl::SimpleAtoi(trace[1], &old_target) &&
      absl::SimpleAtoi(trace[2], &object_id) &&
      absl::SimpleAtoi(trace[3], &new_target) &&
      absl::SimpleAtoi(trace[4], &thread_id)) {
    tree_->HandleObjectUpdate(old_target, object_id,
                               new_target, thread_id);
    return true;
  }
  return false;
}

bool Simulator::ParseMethodEntry(std::vector<std::string> trace) {
  if (trace.size() != 4) {
    return false;
  }

  int method_id;
  int object_id;
  int thread_id;

  if (absl::SimpleAtoi(trace[1], &method_id)&&
      absl::SimpleAtoi(trace[2], &object_id) &&
      absl::SimpleAtoi(trace[3], &thread_id)) {
    tree_->HandleMethodEntry(method_id, object_id, thread_id);
    return true;
  }
  return false;
}

bool Simulator::ParseMethodExit(std::vector<std::string> trace) {
  if (trace.size() != 4) {
    return false;
  }

  int method_id;
  int object_id;
  int thread_id;

  if (absl::SimpleAtoi(trace[1], &method_id)&&
      absl::SimpleAtoi(trace[2], &object_id) &&
      absl::SimpleAtoi(trace[3], &thread_id)){
    tree_->HandleMethodExit(method_id, object_id, thread_id);
    return true;
  }

  return false;
}

bool Simulator::Execute(std::string line) {
    std::vector<std::string> trace = absl::StrSplit(line, ' ');

    if (trace.size() < 1 || trace[0].length() != 1) {
      return false;
    }

    char kind = trace[0][0];

    switch (kind) {
      case 'A':
        return ParseObjectAllocation(trace);
      case 'U':
        return ParseObjectUpdate(trace);
      case 'M':
        return ParseMethodEntry(trace);
      case 'E':
        return ParseMethodExit(trace);
      case 'R':
        // -- Throw out roots for now
        return true;
      default:
        LOG(INFO) << "UNKNOWN";
    }
    return false;
}

void Simulator::ReadTraceFile() {
  std::string line;
  int64_t record_count = 0;

  std::ifstream in;
  in.open(trace_file_);

  if (!trace_file_.empty()) {
    std::ifstream in;
    in.open(trace_file_);

    if (in.fail()) {
      LOG(FATAL) << "Failed to open name file " << trace_file_;
    }

    while (!in.eof()) {
      LOG_EVERY_N(INFO, 1000000) << "At " << record_count;
      getline(in, line);

      if (!Execute(line)) {
        LOG(FATAL) << "Parsing event " << line << " failed.";
      }

      record_count++;
    }

  } else {
    LOG(INFO) << "Traces file was not specified.";
  }
}

// void Simulator::ReadNameFile()
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
// name_file.open(name_file_);
//
// if (name_file.fail()) {
//   std::cout << "Failed to open name file " << name_file_ << std::endl;
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

void Simulator::Simulate() {
  ReadTraceFile();
  Report();
}

void Simulator::Report() {
  // TODO(leandrohw): decide how to output data
}
}  // namespace et_simulator
