load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Third-party
# =========================================================

# Patch to fix asio 1.18.1 being broken in MSVC: https://github.com/chriskohlhoff/asio/pull/584
http_archive(
    name = "asio",  # BSL-1.0
    build_file = "//third_party:asio.BUILD",
    sha256 = "4af9875df5497fdd507231f4b7346e17d96fc06fe10fd30e2b3750715a329113",
    strip_prefix = "asio-1.18.1",
    urls = ["https://downloads.sourceforge.net/project/asio/asio/1.18.1%20(Stable)/asio-1.18.1.tar.bz2"],
    patch_cmds = ["sed -i '238 a inline' include/asio/impl/use_awaitable.hpp"],
)

http_archive(
    name = "boringssl",  # OpenSSL + ISC
    sha256 = "af0e0b561629029332d832fd4a2f2fbed14206eb2f538037a746d82e5281ace8",
    strip_prefix = "boringssl-b92ed690b31bc2df9197dca73b38aaa1a9225a40",
    urls = ["https://github.com/google/boringssl/archive/b92ed690b31bc2df9197dca73b38aaa1a9225a40.tar.gz"],
)
