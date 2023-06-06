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
    sha256 = "226438b0798099ad2a202563a83571ce06dd13b570d8fded4840dbc1f97fa328",
    strip_prefix = "asio-asio-1-28-0",
    url = "https://github.com/chriskohlhoff/asio/archive/asio-1-28-0.tar.gz",
)

# boringssl//:ssl cheats and pulls in private includes from boringssl//:crypto.
http_archive(
    name = "boringssl",  # OpenSSL + ISC
    patch_cmds = ["sed -i '33i package(features=[\"-layering_check\"])' BUILD"],
    sha256 = "62939a56062a3be7417a4e195c8bf0e7f07fdc26a9c49ef42ffd9b4030e3e921",
    strip_prefix = "boringssl-3c13ec0a400cfe5a29c5e0726cecaa51d2bffcc9",
    url = "https://github.com/google/boringssl/archive/3c13ec0a400cfe5a29c5e0726cecaa51d2bffcc9.tar.gz",
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
