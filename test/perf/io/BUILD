load("//test/perf/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps(),
)

cc_binary (
  name = "fastio_bm",
  srcs = ["fastio_bm.cpp"],
  deps = [
    "//hermes/io:fastio",
    "//hermes/random:random",
    ":third_party",
  ]
)

