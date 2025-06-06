#!/bin/bash
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <c_file_name> <k_value> <testcase_numbers>!"
    exit 1
fi

TEST_FILE="$1"  #OK。变量名和等号之间不允许有空格。
MATH_FILE="z2k.sage"
K="$2"
TEST_NUM="$3"

export HOME="$TMPDIR"
sage $MATH_FILE $TEST_NUM $K add >> expected_out_add
sage $MATH_FILE $TEST_NUM $K multiply >> expected_out_multiply
sed -i "/constexpr int k/ s/constexpr int k.*/constexpr int k = $K;/" "$TEST_FILE"

