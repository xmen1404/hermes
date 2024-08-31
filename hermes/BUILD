cc_library (
  name = "hermes",
  deps = [
    "//hermes/fastio:fastio",
    "//hermes/random:random",
    "//hermes/network:network",
    ":third-party",
  ],
  visibility = ["//visibility:public"],
)

cc_library (
  name = "third-party",
  deps = [
    "@com_github_google_glog//:glog",
  ],
  visibility = ["//visibility:public"],
)
