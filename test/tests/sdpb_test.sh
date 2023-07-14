#!/bin/bash

# setup
source test/common_test_setup.sh || exit 1

# set default parameters except for paths
# Usage:
# run_sdpb -s "sdpPath" -c "checkpointDir" -o "outDir"
function run_sdpb() {
  $MPI_RUN_COMMAND build/sdpb --precision=1024 --noFinalCheckpoint --procsPerNode=1 $@
}

# default sdp input file
sdp_path=$TEST_DATA_DIR/sdp.zip
# set default parameters except for paths
# Usage:
# run_sdpb_default_sdp_custom_output_prefix "output_prefix"
function run_sdpb_default_sdp_custom_output_prefix() {
  local output_prefix=$1
  run_sdpb -s $sdp_path -c "$output_prefix/ck" -o "$output_prefix/out" "${*:2}"
}

TEST_RUN_SUCCESS "run" run_sdpb_default_sdp_custom_output_prefix $TEST_OUT_DIR/sdpb
TEST_RUN_SUCCESS "check-output" diff $TEST_OUT_DIR/sdpb/out $TEST_DATA_DIR/sdpb/test_out_orig

# create file and prohibit writing
function touch_no_write() {
  local filename=$1
  local dir_name=$(dirname "$filename")
  rm -rf "$dir_name"
  mkdir -p "$dir_name"
  touch "$filename"
  chmod a-w "$filename"
}

io_tests="$TEST_OUT_DIR/sdpb/io_tests"

touch_no_write $io_tests/write_profile/ck.profiling.0
TEST_RUN_FAILS "write-profile" run_sdpb_default_sdp_custom_output_prefix "$io_tests/write_profile" --maxIterations=1 --verbosity=2
TEST_RUN_FAILS "empty-profiling.0" CHECK_FILE_NOT_EMPTY $io_tests/write_profile/ck.profiling.0
TEST_RUN_SUCCESS "non-empty-profiling.1" CHECK_FILE_NOT_EMPTY $io_tests/write_profile/ck.profiling.1

function test_sdpb_nowrite() {
  local name="$1"
  local output_prefix="$io_tests/$name"
  touch_no_write "$output_prefix/out/$name"
  TEST_RUN_FAILS "nowrite-$name" run_sdpb_default_sdp_custom_output_prefix "$output_prefix" --maxIterations=1 --writeSolution=x,y,X,Y
}

test_sdpb_nowrite "out.txt"
test_sdpb_nowrite "x_0.txt"
test_sdpb_nowrite "y.txt"
test_sdpb_nowrite "X_matrix_0.txt"
test_sdpb_nowrite "Y_matrix_0.txt"

input_corruption="$io_tests/input_corruption"
mkdir -p "$input_corruption"
cp -r $sdp_path "$input_corruption"
perl -p -0777 -i -e 'substr($_,1138,1)^=chr(1<<5)' $input_corruption/sdp.zip
TEST_RUN_FAILS "corrupt-sdp.zip" run_sdpb -s $input_corruption/sdp.zip -c $input_corruption/ck -o $input_corruption/out --maxIterations=1

checkpoint_read="$io_tests/checkpoint_read"
run_sdpb_default_sdp_custom_output_prefix "$checkpoint_read" --maxIterations=1 --writeSolution=x,y,X,Y >/dev/null
chmod a-r $checkpoint_read/out/X_matrix_0.txt
TEST_RUN_FAILS "noread-checkpoint-X_matrix_0.txt" run_sdpb -s $sdp_path -c $checkpoint_read/out -o $checkpoint_read/out_new --maxIterations=1

checkpoint_header=$io_tests/checkpoint_header
run_sdpb_default_sdp_custom_output_prefix "$checkpoint_header" --maxIterations=1 --writeSolution=x,y,X,Y >/dev/null
rm $checkpoint_header/out/X_matrix_0.txt
touch $checkpoint_header/out/X_matrix_0.txt
TEST_RUN_FAILS "empty-text-checkpoint-X_matrix_0.txt" run_sdpb -s $sdp_path -c $checkpoint_header/out -o $checkpoint_header/out_new --maxIterations=1

checkpoint_data=$io_tests/checkpoint_data
run_sdpb_default_sdp_custom_output_prefix $checkpoint_data --maxIterations=1 --writeSolution=x,y,X,Y >/dev/null
head -n 2 $checkpoint_data/out/X_matrix_0.txt >$checkpoint_data/out/X_matrix_0.txt
TEST_RUN_FAILS "corrupt-text-checkpoint-X_matrix_0.txt" run_sdpb -s $sdp_path -c $checkpoint_data/out -o $checkpoint_data/out_new --maxIterations=1
