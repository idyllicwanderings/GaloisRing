
#ifndef _UT_h_
#define _UT_h_

// TODO: include 

void UT_startTest(const char *s, const char *info);
void UT_endTest();
void UT_assert(int condition, char * s);
void UT_displayInfo(const char *header, const char *contents);

#endif