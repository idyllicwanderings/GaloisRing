#!/bin/bash
if [ "$#" -ne 4 ]; then
    echo "Usage: $0  <k_value> <d_value> <d_bigger_value> <testcase_numbers>!!"
    exit 1
fi

MATH_FILE="genGR.sage"
TEST_HDR_FILE="testGR.h"
K = "$1"
D1 = "$2"
D2 = "$3"
TEST_NUM="$4"

sage $MATH_FILE $TEST_NUM $K $D0 $D1 mult add sub inv lift

sed -i "s/#define k .*/#define k $K/" "$TEST_HDR_FILE"
sed -i "s/#define d1 .*/#define d1 $D1/" "$TEST_HDR_FILE"
sed -i "s/#define d2 .*/#define d2 $D2/" "$TEST_HDR_FILE"


