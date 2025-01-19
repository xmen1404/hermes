load("//test/perf/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps(),
)

cc_binary (
  name = "write_bm",
  srcs = ["write_bm.cpp"],
  deps = [
    "//hermes/container:container",
    ":third_party",
  ],
)
