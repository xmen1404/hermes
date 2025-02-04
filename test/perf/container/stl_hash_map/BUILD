load("//test/perf/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps(),
)

cc_binary (
  name = "insert_bm",
  srcs = ["insert_bm.cpp"],
  deps = [
    "//hermes/container:container",
    ":third_party",
  ],
)

cc_binary (
  name = "random_access_bm",
  srcs = ["random_access_bm.cpp"],
  deps = [
    "//hermes/container:container",
    "//hermes/random:random",
    ":third_party",
  ],
)

cc_binary (
  name = "erase_bm",
  srcs = ["erase_bm.cpp"],
  deps = [
    "//hermes/container:container",
    "//hermes/random:random",
    ":third_party",
  ],
)

