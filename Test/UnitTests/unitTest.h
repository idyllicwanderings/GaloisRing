
#ifndef __UT_h_
#define __UT_h_


#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

void UT_startTest(const char *s, const char *info);
void UT_endTest();
void UT_assert(int condition, char * s);
void UT_displayInfo(const char *header, const char *contents);

void UT_testVectorParse(const char * tv_file, std::vector<std::string>& in_a, std::vector<std::string>& in_b, std::vector<std::string>& in_c);

#endif