cc_library (
  name = "zmq",
  hdrs = glob([
    "*.h",
    "*.hpp",
  ]),
  srcs = ["libzmq.a"],
  linkstatic = 1,
  linkopts = ["-lgnutls", "-lbsd"],
  visibility = ["//visibility:public"],
)
