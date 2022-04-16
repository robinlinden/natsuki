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

# Benchmark listeners
cc_library(
    name = "ibenchmark_listener",
    hdrs = ["ibenchmark_listener.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":options",
        ":partial_result",
    ],
)

cc_library(
    name = "stdout_listener",
    hdrs = ["stdout_listener.h"],
    visibility = ["//visibility:public"],
    deps = [":ibenchmark_listener"],
)

# Application
cc_binary(
    name = "bench",
    srcs = ["bench.cc"],
    deps = [
        ":arg_parser",
        ":ibenchmark_listener",
        ":options",
        ":partial_result",
        ":stdout_listener",
        "//natsuki",
    ],
)
