#include <fstream>
#include <cstdlib>
#include <string>
#include <iostream>

#include "gtest/gtest.h"
// #include "lib/gring.h"
#include <glog/logging.h>

constexpr int k = 5;
constexpr int d = 5;

TEST(BRTest, Add) {
    ifstream out_file;
    out_file.open("test/br/expected_out_add", ios::in);
    std::string a, b, c;
    while(!out_file.eof()) {
        out_file >> a;
        out_file >> b;
        out_file >> c;
        // TODO: 写一个polynomial parser
        
        BR<k, d> A(std::stoi(a));
        BR<k, d> B(std::stoi(b));
        BR<k, d> C(std::stoi(c));
        EXPECT_EQ(C, A + B);
    }
}

TEST(BRTest, Multiply) {
    ifstream out_file;
    out_file.open("test/br/expected_out_multiply", ios::in);
    std::string a, b, c;
    while(!out_file.eof()) {
        out_file >> a;
        out_file >> b;
        out_file >> c;
        BR<k, d> A(std::stoi(a));
        BR<k, d> B(std::stoi(b));
        BR<k, d> C(std::stoi(c));
        EXPECT_EQ(C, A * B);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}