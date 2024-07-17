
#ifndef _UT_h_
#define _UT_h_

// TODO: include 

void UT_startTest(const char *s, const char *info);
void UT_endTest();
void UT_assert(int condition, char * s);
void UT_displayInfo(const char *header, const char *contents);

void UT_testVectorParse(const char * tv_file, std::vector<std::string> in_a, std::vector<std::string> in_b, std::vector<std::string> in_c);
#endif