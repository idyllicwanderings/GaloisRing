#!/bin/bash
if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <file_name> <k_value> <d_value> <testcase_numbers>!!"
    exit 1
fi

TEST_FILE="$1"  
MATH_FILE="br.sage"
K="$2"
D="$3"
TEST_NUM="$4"

sage $MATH_FILE $TEST_NUM $K $D add >> expected_out_add
sage $MATH_FILE $TEST_NUM $K $D multiply >> expected_out_multiply
sed -i "/constexpr int k/ s/constexpr int k.*/constexpr int k = $K;/" "$TEST_FILE"
sed -i "/constexpr int d/ s/constexpr int k.*/constexpr int d = $D;/" "$TEST_FILE"

