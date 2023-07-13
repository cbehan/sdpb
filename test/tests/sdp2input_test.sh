#!/bin/bash

# setup
source test/common_test_setup.sh || exit 1

input_dir=$TEST_DATA_DIR/sdp2input
output_dir=$TEST_OUT_DIR/sdp2input
rm -rf $output_dir

function zip_summary() {
    local filename="$1"
    (unzip -vqq "$filename"  | awk '{$2=""; $3=""; $4=""; $5=""; $6=""; print}' | sort -k3 -f )
}

# compare two sdp.zip archives by content ignoring control.json (it contains command which can be different)
function diff_zip_ignore_control() {
  local x=$1
  local y=$2
  diff --exclude=control.json \
    <(zip_summary "$x" | grep -v "control.json") \
    <(zip_summary "$y" | grep -v "control.json")
}

function run_sdp2input() {
  local filename=$1
  # note that input files contain 214 decimal digits (~710 binary)
  # to have same results for .m and json, we have to set precision <=710
  mpirun -n 2 build/sdp2input --precision=512 --input="$input_dir/$filename" --output="$output_dir/$filename/sdp.zip"
}

function sdp2input_run_test() {
  local filename=$1
  local result=$output_dir/$filename/sdp.zip
  local orig=$input_dir/sdp_orig.zip
  TEST_RUN_SUCCESS "run sdp2input $filename" run_sdp2input "$filename"
  TEST_RUN_SUCCESS "check sdp2input result for $filename" diff_zip_ignore_control "$result" "$orig"
}

sdp2input_run_test "sdp2input_test.json"
sdp2input_run_test "sdp2input_split.nsv"
sdp2input_run_test "sdp2input_test.m"