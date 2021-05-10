load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "natsuki",
    srcs = ["natsuki.cpp", "natsuki.h"],
    visibility = ["//visibility:public"],
    deps = ["@asio"],
)
