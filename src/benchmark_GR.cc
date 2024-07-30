#include "../lib/gring.h"

#include <chrono>
#include <iostream>

uint64_t nanos()
{
    uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    return ns; 
}

void GRT_mult_benchmark() {
    #define k 7
    #define d1 2
    #define d2 3
    #define d3 2
    // grab random values for a, b, and c
    GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> a = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::random();
    GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> b = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::random();
    auto start = nanos();
    // perform multiplication
    auto result = a * b;
    auto stop = nanos();
    auto duration = stop - start;

    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "b " << b.force_str() << std::endl;
    // std::cout << "a * b " << result.force_str() << std::endl;

    std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
        << ") multiplication took " << duration << " nanoseconds" << std::endl;
    
    #undef k
    #undef d0
    #undef d1
    #undef d2
}

void GRT_add_benchmark() {
    #define k 7
    #define d1 2
    #define d2 3
    #define d3 2
    // grab random values for a, b, and c
    GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> a = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::random();
    GRT1e<GRT1e<GR1e<k, d1>, d2>, d3> b = GRT1e<GRT1e<GR1e<k, d1>, d2>, d3>::random();
    auto start = nanos();
    // perform addition
    auto result = a + b;
    auto stop = nanos();
    auto duration = stop - start;

    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "b " << b.force_str() << std::endl;
    // std::cout << "a + b " << result.force_str() << std::endl;

    std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
        << ") addition took " << duration << " nanoseconds" << std::endl;
    
    #undef k
    #undef d0
    #undef d1
    #undef d2
}

void GR_mult_benchmark() {
    #define k 7
    #define d1 12
    // grab random values for a, b, and c
    GR1e<k, d1> a = GR1e<k, d1>::random();
    GR1e<k, d1> b = GR1e<k, d1>::random();
    
    // perform multiplication
    auto start = nanos();
    auto result = a * b;
    auto stop = nanos();

    auto duration = stop - start;

    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "b " << b.force_str() << std::endl;
    // std::cout << "a * b " << result.force_str() << std::endl;

    std::cout << "GR(" << k << ", " << d1 << ") multiplication took " << duration << " nanoseconds" << std::endl;
    
    #undef k
    #undef d0
    #undef d1
    #undef d2
}

void GR_add_benchmark() {
    #define k 7
    #define d1 12
    // grab random values for a, b, and c
    GR1e<k, d1> a = GR1e<k, d1>::random();
    GR1e<k, d1> b = GR1e<k, d1>::random();
    auto start = nanos();
    // perform addition
    auto result = a + b;
    auto stop = nanos();
    auto duration = stop - start;

    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "b " << b.force_str() << std::endl;
    // std::cout << "a + b " << result.force_str() << std::endl;

    std::cout << "GR(" << k << ", " << d1 << ") addition took " << duration << " nanoseconds" << std::endl;
    
    #undef k
    #undef d0
    #undef d1
    #undef d2
}

int main() {

    std::cout << " =========================== gr towers benchmark =========================== " << std::endl;
    // GRT_mult_benchmark();
    // GRT_add_benchmark();
    std::cout << " =========================== gr w/o towers benchmark ======================== " << std::endl;
    GR_mult_benchmark();
    // GR_add_benchmark();


    return 1;
}
