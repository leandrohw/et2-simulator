# Heap Simulator
C++ program for reading ET2 trace files and computing precise death times

## Requirements
* [bazel](https://docs.bazel.build/versions/master/install.html)

## Installation
* bazel build //simulator:main

## Running
* bazel-bin/simulator/main

## Testing
To run all test cases, execute:
* bazel test //simulator/...
To run an individual test case, run one of the following:
* bazel test //simulator:ccnode_test
* bazel test //simulator:cctree_test
* bazel test //simulator:heap_test
* bazel test //simulator:simulator_test

