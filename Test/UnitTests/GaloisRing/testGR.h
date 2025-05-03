#ifndef _TEST_GRTOWERING_H_
#define _TEST_GRTOWERING_H_

#include "../UnitTest.h"
#include "../lib/gring.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <chrono>
#include <functional>

#define k 7
#define d1 2
// d0 used for lifting
#define d2 6 

#define ADD_TV_FILE "test/TestVectors/GRAddition.txt"
#define SUB_TV_FILE "test/TestVectors/GRSubtraction.txt"
#define MUL_TV_FILE "test/TestVectors/GRMultiplication.txt"
#define INV_TV_FILE "test/TestVectors/GRInverse.txt"
#define LIFT_TV_FILE "test/TestVectors/GRLift.txt"


#ifdef UT_OUTPUT
void writeTestGR(void);
#endif


void testGR(void);

void selfTestAddition(std::string in_a, std::string in_b, std::string expected_c);

void selfTestSubtraction(std::string in_a, std::string in_b, std::string expected_c);

void selfTestMultiplication(std::string in_a, std::string in_b, std::string expected_c);

void selfTestInverse(std::string in_a, std::string in_b);

void selfTestLift(std::string in_a, std::string in_b);

#endif
