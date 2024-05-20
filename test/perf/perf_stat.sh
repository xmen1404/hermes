#!/bin/bash

# NOTE: run this script from WORKSPACE directory

# Check if a source file is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <target>. To generate new input data run: $0 <target> true"
    exit 1
fi

# Extract input generate flag
TO_GENERATE_INPUT=${2:true}

# Set the base name
BASE_NAME=$1

# Define bazel target name
GEN_TARGET_NAME="//test/data:${BASE_NAME}_perf_input"
PERF_TARGET_NAME="//test/perf:${BASE_NAME}_perf"

# Define the generated output filename and output directory to store the file
INPUT_FILE="${BASE_NAME}_perf.input"
INPUT_DIR="./test/data/input"

# Define perf binary output to run `perf stat`
PERF_TARGET_OUTPUT="bazel-bin/test/perf/${BASE_NAME}_perf"

# Define perf stat log directory
LOG_PATH="./test/perf/log/${BASE_NAME}.log"

# Only store last 250 run records
echo "$(tail -n 5000 ${LOG_PATH})" > $LOG_PATH

if [ "$TO_GENERATE_INPUT" = "true" ]; then
  # run the gen file
  bazel build $GEN_TARGET_NAME

  if [ -f "bazel-bin/test/data/${INPUT_FILE}" ]; then
    echo "${INPUT_FILE} generated successfully. Copying to ${INPUT_DIR}"
    sudo cp bazel-bin/test/data/$INPUT_FILE $INPUT_DIR

    if [ $? -eq 0  ]; then
      echo "${INPUT_FILE} copied to ${INPUT_DIR} successfully."
    else
      echo "Failed to copy ${INPUT_FILE} to ${INPUT_DIR}"
      exit 1
    fi
  fi
fi

# run perf stat
bazel build $PERF_TARGET_NAME

if [ $? -eq 0 ]; then
  echo "${PERF_TARGET_NAME} generated successfully. Running the perf stat."
  sudo perf stat -o $LOG_PATH --append $PERF_TARGET_OUTPUT
  cat $LOG_PATH | tail -n 100
else
  echo "Failed to generate perf target ${PERF_TARGET_NAME}."
fi

