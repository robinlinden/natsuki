load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_binary(
    name = "natsuki_example",
    srcs = ["natsuki_example.cc"],
    deps = [":natsuki"],
)

cc_library(
    name = "natsuki",
    srcs = ["natsuki.cc"],
    hdrs = ["natsuki.h"],
    visibility = ["//visibility:public"],
    deps = ["@asio"],
)

cc_test(
    name = "natsuki_test",
    size = "small",
    srcs = ["natsuki_test.cc"],
    deps = [
        ":natsuki",
        "@hastur//etest",
    ],
)
