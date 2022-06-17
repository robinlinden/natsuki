load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "pthread",
    copts = select({
        "@platforms//os:linux": ["-pthread"],
        "@platforms//os:windows": [],
    }),
    linkopts = select({
        "@platforms//os:linux": ["-pthread"],
        "@platforms//os:windows": [],
    }),
    visibility = ["//visibility:public"],
)
