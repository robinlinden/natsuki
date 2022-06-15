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

http_archive(
    name = "hastur",  # BSD-2-Clause
    sha256 = "b81ae14d9862476c2da799984529b6dd9c6311b0d8f176d047c7149a9195d38c",
    strip_prefix = "hastur-25c528e1c9cf22ff56cbaf5a686d7cfa43a94313",
    url = "https://github.com/robinlinden/hastur/archive/25c528e1c9cf22ff56cbaf5a686d7cfa43a94313.tar.gz",
)

http_archive(
    name = "natsc",  # Apache-2.0
    build_file = "//third_party:natsc.BUILD",
    sha256 = "16e700d912034faefb235a955bd920cfe4d449a260d0371b9694d722eb617ae1",
    strip_prefix = "nats.c-3.3.0",
    url = "https://github.com/nats-io/nats.c/archive/v3.3.0.tar.gz",
)

# docker run --init -p 7447:7447/tcp -p 8000:8000/tcp eclipse/zenoh:0.5.0-beta.9
# bazel run @zenoh-pico//:zn_sub -- "/demo/example/**" tcp/localhost:7447
# bazel run @zenoh-pico//:zn_pub -- "/demo/example/zenoh-pico-pub" "Pub from pico!" tcp/localhost:7447
http_archive(
    name = "zenoh-pico",  # Apache-2.0
    build_file = "//third_party:zenoh-pico.BUILD",
    sha256 = "7430e3bbd63ea8322491c6e5ade3da4e28b53cabef8cb39529f41dc1254194f3",
    strip_prefix = "zenoh-pico-0.5.0-beta.9",
    url = "https://github.com/eclipse-zenoh/zenoh-pico/archive/0.5.0-beta.9.tar.gz",
)
