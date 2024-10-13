load("//hermes/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps()
)

cc_library (
  name = "container",
  srcs = glob(["*.h"]),
  deps = [":third_party"], 
  visibility = ["//visibility:public"]
)

