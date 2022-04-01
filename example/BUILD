load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "natsuki",
    srcs = ["natsuki.cc"],
    deps = ["//natsuki"],
)

cc_binary(
    name = "bench",
    srcs = ["bench.cc"],
    deps = ["//natsuki"],
)
