# integral_fastio_perf
cc_binary (
  name = "integral_fastio_perf_gen",
  srcs = ["integral_fastio_perf_gen.cpp"],
  deps = ["//hermes:hermes"]
)

genrule(
    name = "integral_fastio_perf_input",
    srcs = [":integral_fastio_perf_gen"],
    outs = ["integral_fastio_perf.input"],
    cmd = "$(location :integral_fastio_perf_gen) && cp integral_fastio_perf.input $(OUTS)",
    visibility = ["//visibility:public"],
)
