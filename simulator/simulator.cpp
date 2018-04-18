#include "simulator/simulator.h"
#include "glog/logging.h"

namespace et_simulator {
void Simulator::read_trace_file(std::ifstream & in) {
  int64_t record_count = 0;
  char kind;
  std::string line;
  int object_id;
  int method_id;
  int size;
  int thread_id;
  std::string type;
  int old_target;
  int new_target;
  int root_id;

  while (!in.eof()) {
    in >> kind;
    if (in.fail())
      break;

    switch (kind) {
      case 'A':
          in >> std::hex >> object_id;
          in >> std::hex >> size;
          in >> type;
          in >> std::hex >> thread_id;
          tree->handle_object_allocation(object_id, size, type,
                                         thread_id, method_id);
        break;
      case 'D':
          in >> std::hex >> object_id;
          tree->handle_object_death(object_id);
        break;
      case 'U':
        in >> std::hex >> old_target;
        in >> std::hex >> object_id;
        in >> std::hex >> new_target;
        in >> std::hex >> thread_id;
        tree->handle_object_update(old_target, object_id,
                                   new_target, thread_id);
        break;
      case 'M':
          in >> std::hex >> method_id;
          in >> std::hex >> object_id;
          in >> std::hex >> thread_id;
          tree->handle_method_entry(method_id, object_id, thread_id);

        break;
      case 'E':
        in >> std::hex >> method_id;
        in >> std::hex >> object_id;
        in >> std::hex >> thread_id;
        tree->handle_method_exit(method_id, object_id, thread_id);
        break;

      case 'R':
        // -- Throw out roots for now
        in >> std::hex >> root_id;
        in >> std::hex >> thread_id;
        break;

      default:
        LOG(INFO) << "UNKNOWN";
        break;
    }

    getline(in, line);

    record_count++;
    LOG_EVERY_N(INFO, 1000000) << "At " << record_count;
  }
}

// void Simulator::read_name_file(std::ifstream & name_file)
// {
//   while (!name_file.eof()) {
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
  std::ifstream name_file;
  name_file.open(namesfile);

  if (name_file.fail()) {
    LOG(FATAL) << "Failed to open name file " << namesfile << std::endl;
  }

  std::ifstream in;
  in.open(tracefile);
  if (in.fail()) {
    LOG(FATAL) << "Failed to open name file " << tracefile << std::endl;
  }

  read_trace_file(in);
}

void Simulator::report() {}

}  // namespace et_simulator
