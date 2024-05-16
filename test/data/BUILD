cc_binary (
  name = "integral_fastio_test_gen",
  srcs = ["integral_fastio_test_gen.cpp"],
  deps = ["//hermes:hermes"]
)

genrule(
    name = "integral_fastio_test_data",
    srcs = [":integral_fastio_test_gen"],
    outs = ["integral_fastio_test.input"],
    cmd = "$(location :integral_fastio_test_gen) && cp integral_fastio_test.input $(OUTS)",
    visibility = ["//visibility:public"],
)
