#TODO: test genrule
genrule(
    name = "z2k-update-testfile",
    srcs = ["z2k.cc", 
        "z2k-update-test.sh"],
    outs = ["z2k-param.cc"],  
    cmd = ''' ./$(location z2k-update-test.sh) $(location z2k.cc) $(k) $(testcase_num)
        && cp $(location z2k-param.cc) && echo \"WHAT THE FUCK!\" 
        ''',
    # tags = ["manual"],  
)

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
    data = ["expected_out_add",
            "expected_out_multiply",
            # "z2k-update-test.sh"
            ],
    tags = ["requires-update"],
)

