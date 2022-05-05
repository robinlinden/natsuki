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
    name = "plain_text_reporter",
    hdrs = ["plain_text_reporter.h"],
    visibility = ["//visibility:public"],
    deps = [":ibenchmark_listener"],
)

# Benchmark interfaces
cc_library(
    name = "pubsub_interface",
    hdrs = [
        "ipublisher.h",
        "isubscriber.h",
    ],
    visibility = ["//visibility:public"],
)

# Natsuki implementation
cc_library(
    name = "natsuki_pubsub",
    hdrs = glob(["natsuki_*.h"]),
    visibility = ["//visibility:public"],
    deps = [":pubsub_interface"],
)

# nats.c implementation
cc_library(
    name = "natsc_pubsub",
    srcs = glob(["natsc_*.cc"]),
    hdrs = glob(["natsc_*.h"]),
    visibility = ["//visibility:public"],
    deps = [
        ":pubsub_interface",
        "@natsc",
    ],
)

# Library
cc_library(
    name = "lib",
    srcs = ["bench.cc"],
    hdrs = ["bench.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":ibenchmark_listener",
        ":options",
        ":partial_result",
        ":pubsub_interface",
    ],
)

# Application
cc_binary(
    name = "bench",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":arg_parser",
        ":lib",
        ":natsuki_pubsub",
        ":plain_text_reporter",
        "//natsuki",
    ],
)
