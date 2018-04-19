cc_library(
    name = "ccnode",
    srcs = ["ccnode.cpp"],
    hdrs = [
        "ccnode.h",
        "method.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "heap",
    srcs = ["heap.cpp"],
    hdrs = ["heap.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":ccnode",
    ],
)

cc_library(
    name = "cctree",
    srcs = ["cctree.cpp"],
    hdrs = [
        "cctree.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":ccnode",
        ":heap",
    ],
)

cc_library(
    name = "simulator",
    srcs = ["simulator.cpp"],
    hdrs = ["simulator.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":cctree",
    ],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [":simulator"],
)

cc_test(
    name = "ccnode_test",
    srcs = ["ccnode_test.cpp"],
    deps = [
        "//simulator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "cctree_test",
    srcs = ["cctree_test.cpp"],
    deps = [
        "//simulator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "heap_test",
    srcs = ["heap_test.cpp"],
    deps = [
        "//simulator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "simulator_test",
    srcs = ["simulator_test.cpp"],
    deps = [
        "//simulator",
        "@com_google_googletest//:gtest_main",
    ],
)
