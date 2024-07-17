// TODO: #include "config.h"
#ifdef Has_GRT

#include <assert>
#include <stdint>
#include <stdlib>
#include <string>

#define case 100

#include "testGRTowering.inc"

void writeTestGRT() {

}



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
    UT_startTest("GRTowering addition with k higher than 64 bits");
    //TODO
    UT_endTest();
#else 
    UT_startTest("GRTowering addition with k less than 64 bits");

    std::vector<std::string> in_a;
    std::vector<std::string> in_b;
    std::vector<std::string> in_c;
    UT_testVectorParse("GRToweringAddition.txt", in_a, in_b, in_c);
    for (int i = 0; i < in_a.size(); i++) {
        selfTestAddition(in_a[i], in_b[i], in_c[i]);
    }
    in_a.clear();
    in_b.clear();
    in_c.clear();

    UT_testVectorParse("GRToweringSubtraction.txt", in_a, in_b, in_c);
    for (int i = 0; i < in_a.size(); i++) {
        selfTestSubtraction(in_a[i], in_b[i], in_c[i]);
    }
    in_a.clear();
    in_b.clear();
    in_c.clear();

    UT_testVectorParse("GRToweringMultiplication.txt", in_a, in_b, in_c);
    for (int i = 0; i < in_a.size(); i++) {
        selfTestMultiplication(in_a[i], in_b[i], in_c[i]);
    }
    in_a.clear();
    in_b.clear();
    in_c.clear();

    UT_testVectorParse("GRToweringInverse.txt", in_a, in_b, in_c);
    for (int i = 0; i < in_a.size(); i++) {
        selfTestInverse(in_a[i], in_b[i]);
    }
    in_a.clear();
    in_b.clear();
    in_c.clear();

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
}

void selfTestSubtraction(unsigned char * in_a, unsigned char * in_b, unsigned char *expected_c) {
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
}

void selfTestMultiplication(unsigned char * in_a, unsigned char * in_b, unsigned char *expected_c) {
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

void selfTestInverse(unsigned char * in_a, unsigned char *expected_a_inv) {
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
    assert(a_inv.force_str() == a.inv().force_str());
}


void selfTestExeceptionalSequence() {
    //TODO
}

void selfTestLift() {
    //TODO
}





