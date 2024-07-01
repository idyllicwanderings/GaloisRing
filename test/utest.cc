#include <cstdlib>
#include <string>
#include <iostream>
#include "utest.h"

void UT_startTest(const char *synopsis, const char *implementation)
{
    std::cout << synopsis <<"," << implementation << std::cout;
}

void UT_endTest()
{
    std::cout "    - OK" << std::cout;
    cin.clear();
    cin.sync();
}

void UT_displayInfo(const char *header, const char *contents)
{
    std::cout << header <<"," << contents << std::cout;
}

void UT_assert(int condition, char * synopsis)
{
    if (!condition) {
        std::cout << synopsis << std::cout;
        exit(1);
    }
}
