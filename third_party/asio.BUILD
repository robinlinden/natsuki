load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "asio",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob([
        "include/**/*.hpp",
        "include/**/*.ipp",
    ]),
    defines = ["ASIO_SEPARATE_COMPILATION"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "@boringssl//:ssl",
        "@pthread",
    ],
)
