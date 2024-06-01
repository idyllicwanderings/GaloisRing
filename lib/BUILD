load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "gen-table",
    hdrs = ["aes.h",
            "random.h",
            "gring.h"],
    visibility = ["//main:__pkg__",
                  "//log:__pkg__",
                  "//test:__subpackages__",
                ],
    deps = [
        "@com_github_gflags_gflags//:gflags",
        "@glog",
    ],
)
