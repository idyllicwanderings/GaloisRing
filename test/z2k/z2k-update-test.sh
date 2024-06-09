#!/bin/bash
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <k_value> <testcase_numbers>!!"
    exit 1
fi

TEST_FILE="z2k.cc"  #OK。脚本中变量名和等号之间不允许有空格。
MATH_FILE="z2k.sage"
K="$1"
TEST_NUM="$2"

sage $MATH_FILE $TEST_NUM $K add >> expected_out_add
sage $MATH_FILE $TEST_NUM $K multiply >> expected_out_multiply
sed -i "/constexpr int k/ s/constexpr int k.*/constexpr int k = $K;/" "$TEST_FILE"

