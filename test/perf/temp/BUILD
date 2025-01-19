load("//test/perf/common:deps.bzl", "third_party_deps")

cc_library (
  name = "third_party",
  deps = third_party_deps(),
)

cc_binary (
  name = "job_manager_bm",
  srcs = ["job_manager_bm.cpp", "job_manager.h"],
  deps = [
    "//hermes/random:random",
    ":third_party",
  ],
  data = ["job.txt"],
)
