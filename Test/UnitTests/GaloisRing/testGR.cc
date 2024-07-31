#include "testGR.h"

void testGR()
{
    
#ifdef UT_OUTPUT
void writeTestGR(void)
{
    FILE *f;
    unsigned int rho;
    //TODO

    f = fopen("TestGR.txt", "w");
    assert(f != NULL);
    // for(rho = 1; rho < 200/8; ++rho)
    //     writeTestSpongePRG(f, rho);
    fclose(f);
}
#endif

#ifdef GR_HAS_K_64BITS_HIGHER
    UT_startTest("GR addition ", "k > 64 bits");
    //TODO
    UT_endTest();
#else 

    std::vector<std::string> in_a;
    std::vector<std::string> in_b;
    std::vector<std::string> in_c;

    // UT_startTest("GR addition", "k < 64 bits");
    // UT_testVectorParse(ADD_TV_FILE, in_a, in_b, in_c);
    // std::cout << in_a.size() << std::endl;
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestAddition(in_a[i], in_b[i], in_c[i]);
    // }
    // UT_displayInfo("GR  test: addition", " testcases passed");
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();

    // ////////////////////////////////////////////////////////////////
    // UT_startTest("GR subtraction", "k < 64 bits");
    // UT_testVectorParse(SUB_TV_FILE, in_a, in_b, in_c);
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestSubtraction(in_a[i], in_b[i], in_c[i]);
    // }
    // UT_displayInfo("GR  test: subtractions", in_a.size() + " testcases passed");
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();

    // //////////////////////////////////////////////////////////////////
    // UT_startTest("GR multiplication", "k < 64 bits");
    // UT_testVectorParse(MUL_TV_FILE, in_a, in_b, in_c);
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestMultiplication(in_a[i], in_b[i], in_c[i]);
    // }
    // UT_displayInfo("GR  test: mult", in_a.size() + " testcases passed");
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();

    /////////////////////////////////////////////////////////////////////
    
    UT_testVectorParse(INV_TV_FILE, in_a, in_b, in_c);
    for (int i = 0; i < in_a.size(); i++) {
        selfTestInverse(in_a[i], in_b[i]);
    }
    in_a.clear();
    in_b.clear();
    in_c.clear();
    UT_displayInfo("GR  test: inverse", in_a.size() + " testcases passed");

    // /////////////////////////////////////////////////////////////////////
    // UT_startTest("GR lift", "k < 64 bits");
    // UT_testVectorParse(LIFT_TV_FILE, in_a, in_b, in_c);
    // for (int i = 0; i < in_a.size(); i++) {
    //     selfTestLift(in_a[i], in_b[i]);
    // }

    // UT_displayInfo("GR  test: lift", in_a.size() + " testcases passed");
    // in_a.clear();
    // in_b.clear();
    // in_c.clear();

    // /////////////////////////////////////////////////////////////////////


    UT_endTest();
#endif
}

void selfTestAddition(std::string in_a, std::string in_b, std::string expected_c) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);
    GR1e<k, d1> c = GR1e<k, d1>::from_list(expected_c);
    auto res = a + b;
    UT_displayInfo("c ", c.force_str().c_str());
    UT_displayInfo("a + b ", res.force_str().c_str());
    assert(c.force_str() == res.force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");
}

void selfTestSubtraction(std::string in_a, std::string in_b, std::string expected_c) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);
    GR1e<k, d1> c = GR1e<k, d1>::from_list(expected_c);
    auto res = a - b;
    UT_displayInfo("c ", c.force_str().c_str());
    UT_displayInfo("a - b ", res.force_str().c_str());
    assert (c.force_str() == res.force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");

}

void selfTestMultiplication(std::string in_a, std::string in_b, std::string expected_c) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);
    GR1e<k, d1> c = GR1e<k, d1>::from_list(expected_c);
    auto res = a * b;

    UT_displayInfo("a ", a.force_str().c_str());
    UT_displayInfo("b ", b.force_str().c_str());
    UT_displayInfo("c ", c.force_str().c_str());
    UT_displayInfo("a * b ", res.force_str().c_str());

    assert(c.force_str() == (a * b).force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");
}

void selfTestInverse(std::string in_a, std::string in_b) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d1> b = GR1e<k, d1>::from_list(in_b);

    UT_displayInfo("a ", a.force_str().c_str());
    UT_displayInfo("expected a ", b.force_str().c_str());
    auto res = a.inv();
    UT_displayInfo("a.inv() ", res.force_str().c_str());

    assert(b.force_str() == res.force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");
}


void selfTestLift(std::string in_a, std::string in_b) {
    GR1e<k, d1> a = GR1e<k, d1>::from_list(in_a);
    GR1e<k, d2> b = GR1e<k, d2>::from_list(in_b);
    auto res = liftGR<k, d1, d2>(a);
    
    UT_displayInfo("a ", a.force_str().c_str());
    UT_displayInfo("b ", b.force_str().c_str());
    UT_displayInfo("lift(a) ", res.force_str().c_str());

    assert(b.force_str() == res.force_str());
    UT_displayInfo("------------------------------------- PASSED ", "-------------------------------------");
}



int main()
{
    testGR();
    return 0;
}


