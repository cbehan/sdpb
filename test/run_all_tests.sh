#!/bin/bash

start_time=$(date +%s)

# Set working directory to sdpb root
script_path="$0" # sdpb/test/run_all_test.sh
cd "$(dirname "$script_path")" # sdpb/test/
cd .. # sdpb/
echo "root directory: $PWD"

# setup
source test/common_test_setup.sh || { echo "Cannot find $PWD/test/common_test_setup.sh"; exit 1; }

for file in test/tests/*.sh
do
  echo "================"
  echo "$file"
  chmod +x "$file"
  echo "----------------"
  source "$file"
done

echo "================"
echo "================"
ECHO_GREEN "PASSED $TEST_PASSED_COUNT TESTS"
ECHO_RED "FAILED $TEST_FAILED_COUNT TESTS"
if [ $TEST_FAILED_COUNT != 0 ]; then
  ECHO_RED "FAILED TESTS: $TEST_FAILED_LIST"
fi

end_time=$(date +%s)
elapsed_time=$((end_time-start_time))
echo "Elapsed time: $elapsed_time seconds"

exit $TEST_FAILED_COUNT