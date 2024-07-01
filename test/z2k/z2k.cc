// #include <fstream>
// #include <cstdlib>
// #include <string>
// #include <iostream>

// #include "gtest/gtest.h"
// #include "lib/gring.h"
// #include <glog/logging.h>

// constexpr int k = 15;

// TEST(Z2KTest, Add) {
//     ifstream out_file;
//     out_file.open("test/z2k/expected_out_add", ios::in);
//     //EXPECT_EQ(identifier_, "add");
//     std::string a, b, c;
//     while(!out_file.eof()) {
//         out_file >> a;
//         out_file >> b;
//         out_file >> c;
//         Z2K<k> A(std::stoi(a));
//         Z2K<k> B(std::stoi(b));
//         Z2K<k> C(std::stoi(c));
//         EXPECT_EQ(C, A + B);
//     }
// }


// TEST(Z2KTest, Multiply) {
//     ifstream out_file;
//     out_file.open("test/z2k/expected_out_multiply", ios::in);
//     //EXPECT_EQ(identifier_, "multiply");
//     std::string a, b, c;
//     while(!out_file.eof()) {
//         out_file >> a;

//         out_file >> b;
//         out_file >> c;
//         Z2K<k> A(std::stoi(a));
//         Z2K<k> B(std::stoi(b));
//         Z2K<k> C(std::stoi(c));
//         EXPECT_EQ(C, A * B);
//     }
// }

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }



#include "config.h"
#ifdef Has_Z2K

#include <assert>
#include <stdint>
#include <stdlib>
#include <string>

#define flavor 100

#ifdef XKCP_has_Sponge_Keccak_width200
    #define prefix KeccakWidth200
    #define SnP_width 200
    #include "testSponge.inc"
    #undef prefix
    #undef SnP_width
#endif

void writeTestZ2k() {

}

void selfTestAdd() {

}

void selfTestMult() {
    
}

void testAddition()
{
    
#ifdef UT_OUTPUT
    writeTestSponge();
#endif

    for(flavor = 1; flavor <= 3; flavor++) {
        if (flavor == flavor_OneCall)
            flavorString = "Testing sponge in one call";

#ifdef Z2K_has_Addition_64bits_lower
    UT_startTest("Z2K with k bits lower than 64 bits", flavorString);
    selfTestSponge(32, 768, flavor, "\xd6\x0a\x95\x77\xb8\x75\x75\xab"); /* Keccak[r=32, c=768] */
    selfTestSponge(64, 736, flavor, "\xb7\xb8\xeb\xe0\x28\xa8\x73\xca"); /* Keccak[r=64, c=736] */
    UT_endTest();
#endif

#ifdef Z2K_has_Addition_64bits_higher
#endif

#ifdef 

}


