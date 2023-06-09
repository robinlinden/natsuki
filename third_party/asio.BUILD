load("@rules_cc//cc:defs.bzl", "cc_library")

ASIO_COPTS = select({
    "@platforms//os:linux": [
        "-Wno-null-dereference",
        "-Wno-shadow",
        "-Wno-undef",
    ],
    "@platforms//os:windows": [],
})

cc_library(
    name = "asio",
    srcs = [
        "asio/src/asio.cpp",
        # "asio/src/asio_ssl.cpp",
    ],
    hdrs = glob([
        "asio/include/**/*.hpp",
        "asio/include/**/*.ipp",
    ]),
    copts = ASIO_COPTS,
    defines = ["ASIO_SEPARATE_COMPILATION"],
    strip_include_prefix = "asio/include",
    visibility = ["//visibility:public"],
    deps = [
        # "@boringssl//:ssl",
        "@pthread",
    ],
)
