#!/bin/bash

if ! command -v clang-format &> /dev/null; then
    echo "clang-format not found. Please install clang-format."
    exit 1
fi

start_dir="/home/vspm/dev/cpp-hermes"

echo "Fixing style for $start_dir"
find "$start_dir" -type f -name '*.cpp' -exec clang-format -style=file -i {} \;
find "$start_dir" -type f -name '*.h' -exec clang-format -style=file -i {} \;
find "$start_dir" -type f -name '*.hpp' -exec clang-format -style=file -i {} \;

