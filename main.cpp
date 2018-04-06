#include "simulator.h"

int main(int argc, char * argv[])
{
  if (argc < 3) {
    std::cout << "Usage: cctree <tracefile> <namesfile>" << std::endl;
    exit(0);
  }

  // CCNode * root = new CCNode(0, 0, 0, 0);

  // cout << "Read names..." << endl;

  // ifstream name_file;
  // name_file.open(argv[2]);
  // if (name_file.fail()) {
  //   cout << "Failed to open name file " << argv[2] << endl;
  //   exit(0);
  // }

  // //read_name_file(name_file);

  // cout << "Read events..." << endl;

  // ifstream in;
  // in.open(argv[1]);
  // if (in.fail()) {
  //   cout << "Failed to open name file " << argv[1] << endl;
  //   exit(0);
  // }
  // ofstream out;
  // out.open("out.txt");
  // read_trace_file(in, root);
  // root->emitTreeJSON(out);
}
