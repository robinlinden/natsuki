# SPDX-FileCopyrightText: 2022-2023 Robin Lindén <dev@robinlinden.eu>
#
# SPDX-License-Identifier: MIT

"""Common copts for Natsuki targets."""

NATSUKI_LINUX_WARNING_FLAGS = [
    "-Wall",
    "-Wextra",
    "-pedantic-errors",
    "-Werror",
    "-Wdouble-promotion",
    "-Wformat=2",
    "-Wmissing-declarations",
    "-Wnull-dereference",
    "-Wshadow",
    "-Wsign-compare",
    "-Wundef",
    "-fno-common",
    "-Wnon-virtual-dtor",
    "-Woverloaded-virtual",
    # Common idiom for zeroing members.
    "-Wno-missing-field-initializers",
]

NATSUKI_WINDOWS_WARNING_FLAGS = [
    # More warnings.
    "/W4",
    # Treat warnings as errors.
    "/WX",
]

NATSUKI_COPTS = select({
    "@platforms//os:linux": NATSUKI_LINUX_WARNING_FLAGS,
    "@platforms//os:windows": NATSUKI_WINDOWS_WARNING_FLAGS,
})
