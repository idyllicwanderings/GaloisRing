load("@rules_cc//cc:defs.bzl", "cc_binary") #, "cc_library")

cc_binary(
        name = "galois-ring",
        srcs = [#"demo.cc",
               "testKeccak.cc",  #for test
        ],
        deps = [
        "//lib:gen-table",
        #"@com_github_google_glog//:glog",
        "xkcp//:xkcp",   #for test
        ],
)
