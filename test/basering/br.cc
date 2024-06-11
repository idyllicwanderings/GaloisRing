#include <fstream>
#include <cstdlib>
#include <string>
#include <iostream>

#include "gtest/gtest.h"
// #include "lib/gring.h"
#include <glog/logging.h>

// TODO: extern k,d in brtable.h
// constexpr int d = 5;
// constexpr int k = 5;


// TODO: 用 const BR测试。
TEST(BRTest, Add) {
}

TEST(BRTest, Multiply) {
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}