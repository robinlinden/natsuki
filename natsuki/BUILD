load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("//bzl:copts.bzl", "NATSUKI_COPTS")

cc_binary(
    name = "natsuki_example",
    srcs = ["natsuki_example.cc"],
    copts = NATSUKI_COPTS,
    deps = [":natsuki"],
)

cc_library(
    name = "natsuki",
    srcs = ["natsuki.cc"],
    hdrs = ["natsuki.h"],
    copts = NATSUKI_COPTS,
    visibility = ["//visibility:public"],
    deps = ["@asio"],
)

cc_test(
    name = "natsuki_test",
    size = "small",
    srcs = ["natsuki_test.cc"],
    copts = NATSUKI_COPTS,
    deps = [
        ":natsuki",
        "@hastur//etest",
    ],
)
