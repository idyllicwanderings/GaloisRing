cc_test(
    name = "z2k-test",
    srcs = ["z2k.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:gtest",
        "//lib:gen-table",
        "@com_github_gflags_gflags//:gflags",
        "@glog",
    ],
    data = ["in",
            "expected_out_add",
            "expected_out_multiply",
            ],
)

