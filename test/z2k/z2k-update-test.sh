#!/bin/bash
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <file_name> <k_value> <testcase_numbers>!!"
    exit 1
fi

TEST_FILE="$1"  #OK。脚本中变量名和等号之间不允许有空格。
MATH_FILE="z2k.sage"
K="$2"
TEST_NUM="$3"


sage $MATH_FILE $TEST_NUM $K add >> expected_out_add
sage $MATH_FILE $TEST_NUM $K multiply >> expected_out_multiply
sed -i "/constexpr int k/ s/constexpr int k.*/constexpr int k = $K;/" "$TEST_FILE"

