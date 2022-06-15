load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "zenoh-pico",
    srcs = glob(
        include = ["src/**/*.c"],
        # src/zenoh/session/tx.c doesn't compile due to 'ISO C requires a
        # translation unit to contain at least one declaration.'
        exclude = ["src/system/*/*.c"] + ["src/zenoh/session/tx.c"],
    ) + select({
        "@platforms//os:linux": glob(["src/system/unix/*.c"]),
    }),
    hdrs = glob(["include/**/*.h"]),
    defines = ["ZENOH_DEBUG=0"] + select({
        "@platforms//os:linux": ["ZENOH_LINUX"],
    }),
    linkopts = select({
        "@platforms//os:linux": ["-lpthread"],
    }),
    strip_include_prefix = "include",
    target_compatible_with = ["@platforms//os:linux"],
    visibility = ["//visibility:public"],
)

[cc_binary(
    name = src[13:-2],
    srcs = [src],
    deps = [":zenoh-pico"],
) for src in glob(["examples/net/*.c"])]
