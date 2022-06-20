load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "asio",
    srcs = [
        "src/asio.cpp",
        # "src/asio_ssl.cpp",
    ],
    hdrs = glob([
        "include/**/*.hpp",
        "include/**/*.ipp",
    ]),
    defines = ["ASIO_SEPARATE_COMPILATION"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        # "@boringssl//:ssl",
        "@pthread",
    ],
)
