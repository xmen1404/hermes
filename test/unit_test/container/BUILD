load("//test/unit_test/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps(),
)

cc_test (
  name = "static_list_test",
  srcs = ["static_list_test.cpp"],
  defines = ["CATCH_CONFIG_MAIN"],
  deps = [
    "//hermes/container:container",
    "//hermes/random:random",
    ":third_party",
  ],
)

cc_test (
  name = "stl_vector_test",
  srcs = ["stl_vector_test.cpp"],
  defines = ["CATCH_CONFIG_MAIN"],
  deps = [
    "//hermes/container:container",
    ":third_party",
  ],
)

cc_test (
  name = "stl_shared_ptr_test",
  srcs = ["stl_shared_ptr_test.cpp"],
  defines = ["CATCH_CONFIG_MAIN"],
  deps = [
    "//hermes/container:container",
    ":third_party",
  ],
)

cc_test (
  name = "spsc_queue_test",
  srcs = ["spsc_queue_test.cpp"],
  defines = ["CATCH_CONFIG_MAIN"],
  deps = [
    "//hermes/container:container",
    "//hermes/random:random",
    ":third_party",
  ],
)

cc_test (
  name = "stl_hash_map_test",
  srcs = ["stl_hash_map_test.cpp"],
  defines = ["CATCH_CONFIG_MAIN"],
  deps = [
    "//hermes/container:container",
    "//hermes/random:random",
    ":third_party",
  ],
)

cc_test (
  name = "smart_pointer",
  srcs = ["smart_pointer_test.cpp"],
  defines = ["CATCH_CONFIG_MAIN"],
  deps = [
    "//hermes/container:container",
    "//hermes/random:random",
    ":third_party",
  ],
)
