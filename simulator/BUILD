cc_library(
    name = "simulator",
    srcs = [
        "ccnode.cpp",
        "cctree.cpp",
        "heap.cpp",
        "simulator.cpp",
    ],
    hdrs = [
        "ccnode.h",
        "cctree.h",
        "heap.h",
        "method.h",
        "simulator.h",
    ],
    visibility = ["//visibility:public"],
    deps = ["@com_google_absl//absl/strings"],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [":simulator"],
)
