load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# Utils
cc_library(
    name = "arg_parser",
    hdrs = ["arg_parser.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "options",
    hdrs = ["options.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "partial_result",
    hdrs = ["partial_result.h"],
    visibility = ["//visibility:public"],
)

# Application
cc_binary(
    name = "bench",
    srcs = ["bench.cc"],
    deps = [
        ":arg_parser",
        ":options",
        ":partial_result",
        "//natsuki",
    ],
)
