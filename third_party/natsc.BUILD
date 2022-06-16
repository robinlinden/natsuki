load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "natsc",
    srcs = glob([
        "src/*.c",
        "src/*.h",
    ]) + select({
        "@platforms//os:linux": glob(["src/unix/*.c"]) + ["src/include/n-unix.h"],
        "@platforms//os:windows": glob(["src/win/*.c"]) + ["src/include/n-win.h"],
    }),
    hdrs = [
        "src/nats.h",
        "src/status.h",
        "src/version.h",
    ],
    copts = select({
        "@platforms//os:linux": ["-pthread"],
        "@platforms//os:windows": [],
    }),
    linkopts = select({
        "@platforms//os:linux": ["-pthread"],
        "@platforms//os:windows": [],
    }),
    local_defines = ["_GNU_SOURCE"],
    strip_include_prefix = "src",
    visibility = ["//visibility:public"],
)
