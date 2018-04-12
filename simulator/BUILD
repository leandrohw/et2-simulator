cc_library(
    name = "simulator",
    srcs = ["simulator.cpp", "cctree.cpp", "ccnode.cpp", "heap.cpp"],
    hdrs = ["method.h", "simulator.h", "cctree.h", "ccnode.h", "heap.h"],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [":simulator"]
)
