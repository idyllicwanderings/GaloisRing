#include "testGRTowering.h"


void testGR()
{
    
#ifdef UT_OUTPUT
void writeTestGRTowering(void)
{
    FILE *f;
    unsigned int rho;
    //TODO

    f = fopen("TestGRTowering.txt", "w");
    assert(f != NULL);
    // for(rho = 1; rho < 200/8; ++rho)
    //     writeTestSpongePRG(f, rho);
    fclose(f);
}
#endif

#ifdef GRT_HAS_K_64BITS_HIGHER
    UT_startTest("GRTowering addition ", "k > 64 bits");
    //TODO
    UT_endTest();
#else 
    UT_startTest("GRTowering addition", "k < 64 bits");

    std::vector<std::string> in_a;
    std::vector<std::string> in_b;
    std::vector<std::string> in_c;
    UT_testVectorParse(ADD_TV_FILE, in_a, in_b, in_c);
    std::cout << in_a.size() << std::endl;
    for (int i = 0; i < in_a.size(); i++) {
        selfTestAddition(in_a[i], in_b[i], in_c[i]);
    }
    UT_displayInfo("GR towering test: addition", " testcases passed");
    in_a.clear();
    in_b.clear();
    in_c.clear();

    //////////////////////////////////////////////////////////////////////////

    UT_testVectorParse(SUB_TV_FILE, in_a, in_b, in_c);
    for (int i = 0; i < in_a.size(); i++) {
        selfTestSubtraction(in_a[i], in_b[i], in_c[i]);
    }
    UT_displayInfo("GR towering test: subtractions", in_a.size() + " testcases passed");
    in_a.clear();
    in_b.clear();
    in_c.clear();

    // ////////////////////////////////////////////////////////////////////////////

    // UT_testVectorParse(MUL_TV_FILE, in_a, in_b, in_c);
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestMultiplication(in_a[i], in_b[i], in_c[i]);
    // }
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();
    // UT_displayInfo("GR towering test: mult", in_a.size() + " testcases passed");

    // ////////////////////////////////////////////////////////////////////////////

    // UT_testVectorParse(INV_TV_FILE, in_a, in_b, in_c);
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestInverse(in_a[i], in_b[i]);
    // }
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();
    // UT_displayInfo("GR towering test: inverse", in_a.size() + " testcases passed");

    UT_endTest();
#endif
}

void selfTestAddition(std::string in_a, std::string in_b, std::string expected_c) {
    #ifdef GRT_HAS_2_TOWERS
        GRT1e<GR1e<k, d1>, d2> a = GRT1e<GR1e<k, d1>, d2>::from_list(in_a);
        GRT1e<GR1e<k, d1>, d2> b = GRT1e<GR1e<k, d1>, d2>::from_list(in_b);
        GRT1e<GR1e<k, d1>, d2> c = GRT1e<GR1e<k, d1>, d2>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_3_TOWERS
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> a = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(in_a);
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> b = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(in_b);
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> c = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_4_TOWERS
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> a = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(in_a);
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> b = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(in_b);
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> c = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_5_TOWERS
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> a = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(in_a);
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> b = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(in_b);
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> c = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(expected_c);
    #endif
    assert(c.force_str() == (a + b).force_str());
    UT_displayInfo("c: ", c.force_str().c_str());
    UT_displayInfo("a + b: ", (a + b).force_str().c_str());
}

void selfTestSubtraction(std::string in_a, std::string in_b, std::string expected_c) {
    #ifdef GRT_HAS_2_TOWERS
        GRT1e<GR1e<k, d1>, d2> a = GRT1e<GR1e<k, d1>, d2>::from_list(in_a);
        GRT1e<GR1e<k, d1>, d2> b = GRT1e<GR1e<k, d1>, d2>::from_list(in_b);
        GRT1e<GR1e<k, d1>, d2> c = GRT1e<GR1e<k, d1>, d2>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_3_TOWERS
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> a = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(in_a);
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> b = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(in_b);
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> c = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_4_TOWERS
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> a = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(in_a);
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> b = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(in_b);
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> c = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_5_TOWERS
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> a = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(in_a);
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> b = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(in_b);
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> c = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(expected_c);
    #endif
    assert(c.force_str() == (a - b).force_str());
    UT_displayInfo("c: ", c.force_str().c_str());
    UT_displayInfo("a - b: ", (a - b).force_str().c_str());
}

void selfTestMultiplication(std::string in_a, std::string in_b, std::string expected_c) {
    #ifdef GRT_HAS_2_TOWERS
        GRT1e<GR1e<k, d1>, d2> a = GRT1e<GR1e<k, d1>, d2>::from_list(in_a);
        GRT1e<GR1e<k, d1>, d2> b = GRT1e<GR1e<k, d1>, d2>::from_list(in_b);
        GRT1e<GR1e<k, d1>, d2> c = GRT1e<GR1e<k, d1>, d2>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_3_TOWERS
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> a = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(in_a);
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> b = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(in_b);
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> c = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_4_TOWERS
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> a = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(in_a);
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> b = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(in_b);
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> c = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(expected_c);
    #endif
    #ifdef GRT_HAS_5_TOWERS
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> a = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(in_a);
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> b = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(in_b);
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> c = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(expected_c);
    #endif
    assert(c.force_str() == (a * b).force_str());
}

void selfTestInverse(std::string in_a, std::string in_b) {
    #ifdef GRT_HAS_2_TOWERS
        GRT1e<GR1e<k, d1>, d2> a = GRT1e<GR1e<k, d1>, d2>::from_list(in_a);
        GRT1e<GR1e<k, d1>, d2> b = GRT1e<GR1e<k, d1>, d2>::from_list(in_b);
    #endif
    #ifdef GRT_HAS_3_TOWERS
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> a = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(in_a);
        GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> b = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::from_list(in_b);
    #endif
    #ifdef GRT_HAS_4_TOWERS
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> a = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(in_a);
        GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4> b = GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>::from_list(in_b);
    #endif
    #ifdef GRT_HAS_5_TOWERS
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> a = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(in_a);
        GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5> b = GRT1e<GRT1e<GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>, d4>, d5>::from_list(in_b);
    #endif
    assert(b.force_str() == a.inv().force_str());
    std::cout << "passed!\n";
}

// void selfTestExeceptionalSequence() {
//     //TODO

// }


int main()
{
    testGR();
    return 0;
}


