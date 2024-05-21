load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")


cc_binary(
    name = "galois-ring",
    #srcs = ["hello-world.cc"],
    srcs = ["main.cc"]
    #hdrs = ["gring.h"],
    deps = [
        #":gen-table",
        "//lib:gen-table",
        "@com_github_google_glog//:glog"],
)
