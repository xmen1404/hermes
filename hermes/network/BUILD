load("//hermes/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps(),
)

cc_library (
  name = "network",
  srcs = glob(["*.h"]),
  deps = [
    "//hermes/network/base:base",
    ":third_party",
  ],
  visibility = ["//visibility:public"],
)
