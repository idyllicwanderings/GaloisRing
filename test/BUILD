cc_test(
    name = "arith-test",
    srcs = ["gr_test.cc",
            "br_test.cc",
            "int_test.cc"
            ],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:main",
        "//main:hello-greet",
    ],
)

