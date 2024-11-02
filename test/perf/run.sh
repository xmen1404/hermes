if [ -z "$1" ]; then
  echo "Please provide bazel target"
  exit 1
fi

# sudo cpupower frequency-set --governor performance
bazel run --config=opt $1
# sudo cpupower frequency-set --governor powersave
