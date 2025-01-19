load("//test/perf/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps(),
)

cc_binary (
  name = "push_back_bm",
  srcs = ["push_back_bm.cpp"],
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
    ":third_party",
  ],
)

cc_binary (
  name = "stl_vector_temp",
  srcs = ["stl_vector_temp.cpp"],
  deps = [
    "//hermes/container:container",
    ":third_party",
  ],
)

