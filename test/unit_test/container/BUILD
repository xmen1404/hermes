load("//test/unit_test/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps(),
)

cc_test (
  name = "stl_vector_test",
  srcs = ["stl_vector_test.cpp"],
  defines = ["CATCH_CONFIG_MAIN"],
  deps = [
    "//hermes/container:container",
    ":third_party"
  ],
)
