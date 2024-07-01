
#ifndef _UT_h_
#define _UT_h_

// TODO: include 

void UT_startTest(const char *synopsis, const char *implementation);
void UT_endTest();
void UT_assert(int condition, char * synopsis);
void UT_displayInfo(const char *header, const char *contents);

#endif