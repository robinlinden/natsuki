load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "arg_parser",
    hdrs = ["arg_parser.h"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "bench",
    srcs = ["bench.cc"],
    deps = [
        ":arg_parser",
        "//natsuki",
    ],
)
