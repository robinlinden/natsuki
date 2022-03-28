load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Third-party
# =========================================================

http_archive(
    name = "asio",  # BSL-1.0
    build_file = "//third_party:asio.BUILD",
    sha256 = "6874d81a863d800ee53456b1cafcdd1abf38bbbf54ecf295056b053c0d7115ce",
    strip_prefix = "asio-1.22.1",
    url = "https://downloads.sourceforge.net/project/asio/asio/1.22.1%20(Stable)/asio-1.22.1.tar.bz2",
)

http_archive(
    name = "boringssl",  # OpenSSL + ISC
    sha256 = "6f2b0390dc23be79268da435b276b0ecfffd1adeaf9868d6a68860f9b9adbcb7",
    strip_prefix = "boringssl-ae0ce154470dc7d1e3073ba8adb1ef2b669c6471",
    url = "https://github.com/google/boringssl/archive/ae0ce154470dc7d1e3073ba8adb1ef2b669c6471.tar.gz",
)
