#!/usr/bin/env sh

name=$1
work_dir="$(pwd)"
src_dir="$work_dir/src"
test_dir="$work_dir/test"
build_dir="$work_dir/build"

mkdir -p $build_dir
cc ${test_dir}/${name}.c -o $build_dir/${name} -I${src_dir}
