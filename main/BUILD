load("@rules_cc//cc:defs.bzl", "cc_binary") #, "cc_library")

cc_binary(
    name = "galois-ring",
    srcs = ["demo.cc",
            #"test.cc"
            ],
    #hdrs = ["gring.h"],
    deps = [
        "//lib:gen-table",
        "@com_github_google_glog//:glog",
        ],
)
