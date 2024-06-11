cc_test(
    name = "br-test",
    srcs = ["br.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:gtest",
        "//lib:gen-table",
        "@com_github_gflags_gflags//:gflags",
        "@glog",
    ],
    data = ["expected_out_add",
            "expected_out_multiply",
            # TODO: BUILD ERROR: $HOME environment not found by SAGE
            #":br-update-testfile"
            ],
    tags = ["requires-update"],
)

