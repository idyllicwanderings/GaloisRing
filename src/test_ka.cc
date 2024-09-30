#include "../lib/gring.h"
#include <chrono>   
#include <cassert>
#include <span>

int main() {

    // std::array<int, 8> a = {0, 1, 2, 3, 4, 5, 6, 7};

    // constexpr int nn = 4;
    // std::array<std::span<int>, 2> aa;

    // for (int i = 0; i < 2; i++) {
    //     aa[i] = std::span<int>(a.data() + i * nn, nn);
    // }

    // std::vector<std::span<int>> aa(2);
    // aa[0] = std::span<int>(a.data(), nn);
    // aa[1] = std::span<int>(a.data() + nn, nn);


    randomness::RO ro;
    ro.gen_random_bytes();

    GR1e<64, 8>::MULT_METHOD_ = arith::PLAIN;

    GR1e<64, 8> a = GR1e<64, 8>::random_element(ro);
    GR1e<64, 8> b = GR1e<64, 8>::random_element(ro);

    auto start = std::chrono::high_resolution_clock::now();
    GR1e<64, 8> c1 = a * b;                                          // Multiplication
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

    GR1e<64, 8>::MULT_METHOD_ = arith::KA_ONE_ITER;
    start = std::chrono::high_resolution_clock::now();
    GR1e<64, 8> c2 = a * b;
    end = std::chrono::high_resolution_clock::now();
    std::cout << "KA one iter Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;
    assert(c1 == c2);

    GR1e<64, 8>::MULT_METHOD_ = arith::KA_RECURSIVE;
    start = std::chrono::high_resolution_clock::now();
    GR1e<64, 8> c3 = a * b;
    end = std::chrono::high_resolution_clock::now();
    std::cout << "KA recursive Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;
    assert(c1 == c3);
    
    std::cout << c1.force_str() << std::endl;
    std::cout << c2.force_str() << std::endl;
    std::cout << c3.force_str() << std::endl;
}
