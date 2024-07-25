#include "UnitTest.h"

void UT_startTest(const char *s, const char *info)
{
    std::cout << s <<"," << info << std::endl;
}

void UT_endTest()
{
    std::cout << "    - OK" << std::endl;
    std::cin.clear();
    std::cin.sync();
}

void UT_displayInfo(const char *header, const char *contents)
{
    std::cout << header <<"," << contents << std::endl;
}

void UT_assert(int condition, char * s)
{
    if (!condition) {
        std::cout << s << std::endl;
        exit(1);
    }
}

void UT_testVectorParse(const char * tv_file, std::vector<std::string> in_a, std::vector<std::string> in_b, std::vector<std::string> in_c)
{
    std::ifstream file(tv_file);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string a, b, c;
        std::getline(iss, a, ';');
        if (std::getline(iss, b, ';')) {
            std::getline(iss, c);
        } else {
            std::getline(iss, b);
            c = "";  
        }
        in_a.push_back(a);
        in_b.push_back(b);
        in_c.push_back(c);
    }
    file.close();

}