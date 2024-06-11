#include <fstream>
#include <cstdlib>
#include <string>
#include <iostream>

#include "gtest/gtest.h"
#include "lib/gring.h"
#include <glog/logging.h>

constexpr int k = 15;

TEST(Z2KTest, Add) {
    ifstream out_file;
    out_file.open("test/z2k/expected_out_add", ios::in);
    //EXPECT_EQ(identifier_, "add");
    std::string a, b, c;
    while(!out_file.eof()) {
        out_file >> a;
        out_file >> b;
        out_file >> c;
        Z2K<k> A(std::stoi(a));
        Z2K<k> B(std::stoi(b));
        Z2K<k> C(std::stoi(c));
        EXPECT_EQ(C, A + B);
    }
}


TEST(Z2KTest, Multiply) {
    ifstream out_file;
    out_file.open("test/z2k/expected_out_multiply", ios::in);
    //EXPECT_EQ(identifier_, "multiply");
    std::string a, b, c;
    while(!out_file.eof()) {
        out_file >> a;

        out_file >> b;
        out_file >> c;
        Z2K<k> A(std::stoi(a));
        Z2K<k> B(std::stoi(b));
        Z2K<k> C(std::stoi(c));
        EXPECT_EQ(C, A * B);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}