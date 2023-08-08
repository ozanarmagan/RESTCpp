

cc_library(
    name = "restcpp",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob(["include/*.h"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    # link based on os
    linkopts = select({
        "//conditions:default": ["-lpthread", "-lssl", "-lcrypto"],
        "@platforms//os:windows": ["-lwsock32", "-lstdc++fs"]
    })
)


cc_binary(
    name = "restcpp_test",
    srcs = ["test/test.cpp", "test/routes.h"],
    deps = [":restcpp"],
)