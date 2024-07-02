#include <cstdlib>
#include <string>
#include <iostream>
#include "utest.h"

void UT_startTest(const char *s, const char *info)
{
    std::cout << s <<"," << info << std::cout;
}

void UT_endTest()
{
    std::cout "    - OK" << std::cout;
    std::cin.clear();
    std::cin.sync();
}

void UT_displayInfo(const char *header, const char *contents)
{
    std::cout << header <<"," << contents << std::cout;
}

void UT_assert(int condition, char * s)
{
    if (!condition) {
        std::cout << s << std::cout;
        exit(1);
    }
}
