#include <fstream>
#include <cstdlib>
#include <string>
#include <iostream>

#include "gtest/gtest.h"
#include "lib/gring.h"
#include <glog/logging.h>


// class Z2KTest : public ::testing::Test {
//     protected:
//     virtual void SetUp() { 
//         // std::ifstream infile("test/z2k/in");
//         // if(!infile.is_open())  {
//         //     std::cout << "file not open!" << std::endl;
//         // }
//         // std::string n, d;
//         // infile >> n;
//         // infile >> d;
//         // infile >> identifier_;
//         // assert(identifier_ == "multiply" || identifier_ == "add");
//         // //success system("cat test/z2k/in");
//         // // failed
//         // system(("sage z2k.sage " + n + " " + d + " " + identifier_ +\
//         //         " >> expected_out_" + identifier_).c_str());
//     }
//     std::string identifier_;
// };

// TODO: 写一个bash script，先更改in中的参数，然后跑sage，然后更改z2k.cc参数k，跑test，最后加到.bazelrc
constexpr int k = 13;

TEST(Z2KTest, Add) {
    // TODO: find a way to instantiaze the template k??
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