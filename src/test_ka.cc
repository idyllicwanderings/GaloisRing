#include "../lib/gring.h"
#include <chrono>   
#include <cassert>

int main() {
    randomness::RO ro;
    ro.gen_random_bytes();

    GR1e<64, 8>::KA_MULT_FLAG_ = false;

    GR1e<64, 8> a = GR1e<64, 8>::random_element(ro);
    GR1e<64, 8> b = GR1e<64, 8>::random_element(ro);

    auto start = std::chrono::high_resolution_clock::now();
    GR1e<64, 8> c1 = a * b;                                          // Multiplication
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

    GR1e<64, 8>::KA_MULT_FLAG_ = true;
    start = std::chrono::high_resolution_clock::now();
    GR1e<64, 8> c2 = a * b;
    end = std::chrono::high_resolution_clock::now();
    std::cout << "KA Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

    assert(c1 == c2);
    // GR1e<64, 16> d = liftGR<64, 8, 16>(a);              // Lift
    // GR1e<64, 8> e = GR1e<64, 8>::random_element(ro);    // Random element
}
