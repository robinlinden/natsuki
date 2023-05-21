load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Misc tools
# =========================================================

# HEAD as of 2022-12-17.
http_archive(
    name = "hedron_compile_commands",
    sha256 = "9b5683e6e0d764585f41639076f0be421a4c495c8f993c186e4449977ce03e5e",
    strip_prefix = "bazel-compile-commands-extractor-c6cd079bef5836293ca18e55aac6ef05134c3a9d",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/c6cd079bef5836293ca18e55aac6ef05134c3a9d.tar.gz",
)

# Third-party
# =========================================================

http_archive(
    name = "asio",  # BSL-1.0
    build_file = "//third_party:asio.BUILD",
    sha256 = "6874d81a863d800ee53456b1cafcdd1abf38bbbf54ecf295056b053c0d7115ce",
    strip_prefix = "asio-1.22.1",
    url = "https://downloads.sourceforge.net/project/asio/asio/1.22.1%20(Stable)/asio-1.22.1.tar.bz2",
)

# boringssl//:ssl cheats and pulls in private includes from boringssl//:crypto.
http_archive(
    name = "boringssl",  # OpenSSL + ISC
    patch_cmds = ["sed -i '33i package(features=[\"-layering_check\"])' BUILD"],
    sha256 = "641c62d698e88d838fc8076098645b72ae3dc0ecb791b75282d6618ac424f4b2",
    strip_prefix = "boringssl-80692b63910ff9f3971412ea509449f73a114e18",
    url = "https://github.com/google/boringssl/archive/80692b63910ff9f3971412ea509449f73a114e18.tar.gz",
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
    sha256 = "4b60fd25bbb04dbc82ea09cd9e1df4f975f68e1b2e4293078ae14e01218a22bf",
    strip_prefix = "nats.c-3.6.1",
    url = "https://github.com/nats-io/nats.c/archive/v3.6.1.tar.gz",
)

local_repository(
    name = "pthread",
    path = "third_party/pthread",
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

# Third-party setup
# =========================================================

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
