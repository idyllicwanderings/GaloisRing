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

    #define ITER_TIMES 10


    randomness::RO ro;
    ro.gen_random_bytes();

    GR1e<64, 8>::MULT_METHOD_ = arith::PLAIN;
    
    std::chrono::nanoseconds total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 8> a = GR1e<64, 8>::random_element(ro);
        GR1e<64, 8> b = GR1e<64, 8>::random_element(ro);
        auto start1 = std::chrono::high_resolution_clock::now();
        GR1e<64, 8> c1 = a * b;                                          // Multiplication
        auto end1 = std::chrono::high_resolution_clock::now();
        total += end1 - start1;
        std::cout << "c1: " << c1.force_str() << std::endl;
    }
    std::cout << "================== multiplication for GR<64, 8> ====================" << std::endl;
    std::cout << "Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count()/ITER_TIMES << "ns" << std::endl;

    GR1e<64, 8>::MULT_METHOD_ = arith::KA_ONE_ITER;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 8> a = GR1e<64, 8>::random_element(ro);
        GR1e<64, 8> b = GR1e<64, 8>::random_element(ro);
        auto start2 = std::chrono::high_resolution_clock::now();
        GR1e<64, 8> c2 = a * b;                                          // Multiplication
        auto end2 = std::chrono::high_resolution_clock::now();
        total += end2 - start2;
        std::cout << "c2: " << c2.force_str() << std::endl;
    }
    std::cout << "KA one iter Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count() /ITER_TIMES<< "ns" << std::endl;
    

    GR1e<64, 8>::MULT_METHOD_ = arith::KA_RECURSIVE;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 8> a = GR1e<64, 8>::random_element(ro);
        GR1e<64, 8> b = GR1e<64, 8>::random_element(ro);
        auto start3 = std::chrono::high_resolution_clock::now();
        GR1e<64, 8> c3 = a * b;                                          // Multiplication
        auto end3 = std::chrono::high_resolution_clock::now();
        total += end3 - start3;
        std::cout << "c3: " << c3.force_str() << std::endl;
    }
    std::cout << "KA recursive Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count() /ITER_TIMES<< "ns" << std::endl;

    
    // std::cout << c1.force_str() << std::endl;
    // std::cout << c2.force_str() << std::endl;
    // std::cout << c3.force_str() << std::endl;


    GR1e<64, 15>::MULT_METHOD_ = arith::PLAIN;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 15> a = GR1e<64, 15>::random_element(ro);
        GR1e<64, 15> b = GR1e<64, 15>::random_element(ro);
        auto start4 = std::chrono::high_resolution_clock::now();
        GR1e<64, 15> c4 = a * b;                                          // Multiplication
        auto end4 = std::chrono::high_resolution_clock::now();
        total += end4 - start4;
        std::cout << "c4: " << c4.force_str() << std::endl;
    }
    std::cout << "================== multiplication for GR<64, 15> ====================" << std::endl;
    std::cout << "Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count() /ITER_TIMES<< "ns" << std::endl;

    
    GR1e<64, 15>::MULT_METHOD_ = arith::KA_ONE_ITER;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 15> a = GR1e<64, 15>::random_element(ro);
        GR1e<64, 15> b = GR1e<64, 15>::random_element(ro);
        auto start5 = std::chrono::high_resolution_clock::now();
        GR1e<64, 15> c5 = a * b;                                          // Multiplication
        auto end5 = std::chrono::high_resolution_clock::now();
        total += end5 - start5;
        std::cout << "c5: " << c5.force_str() << std::endl;	
    }
    std::cout << "KA one iter Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count() /ITER_TIMES << "ns" << std::endl;
 

    GR1e<64, 15>::MULT_METHOD_ = arith::KA_RECURSIVE;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 15> a = GR1e<64, 15>::random_element(ro);
        GR1e<64, 15> b = GR1e<64, 15>::random_element(ro);
        auto start6 = std::chrono::high_resolution_clock::now();
        GR1e<64, 15> c6 = a * b;                                          // Multiplication
        auto end6 = std::chrono::high_resolution_clock::now();
        total += end6 - start6;
        std::cout << "c6: " << c6.force_str() << std::endl;
    }
    std::cout << "KA recursive Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count() /ITER_TIMES << "ns" << std::endl;


    GR1e<64, 15>::MULT_METHOD_ = arith::KA_RECURSIVE_DUMMY;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 15> a = GR1e<64, 15>::random_element(ro);
        GR1e<64, 15> b = GR1e<64, 15>::random_element(ro);
        auto start7 = std::chrono::high_resolution_clock::now();
        GR1e<64, 15> c7 = a * b;                                          // Multiplication
        auto end7 = std::chrono::high_resolution_clock::now();
        total += end7 - start7;
        std::cout << "c7: " << c7.force_str() << std::endl;
    }
    std::cout << "KA recursive dummy Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count() /ITER_TIMES<< "ns" << std::endl;

    // std::cout << z1.force_str() << std::endl;
    // std::cout << z2.force_str() << std::endl;
    // std::cout << z3.force_str() << std::endl;
    // std::cout << z4.force_str() << std::endl;

    GR1e<64, 30>::MULT_METHOD_ = arith::PLAIN;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 30> a = GR1e<64, 30>::random_element(ro);
        GR1e<64, 30> b = GR1e<64, 30>::random_element(ro);
        auto start8 = std::chrono::high_resolution_clock::now();
        GR1e<64, 30> c8 = a * b;                                          // Multiplication
        auto end8 = std::chrono::high_resolution_clock::now();
        total += end8 - start8;
        std::cout << "c8: " << c8.force_str() << std::endl;
    }
    std::cout << "================== multiplication for GR<64, 30> ====================" << std::endl;
    std::cout << "Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count() /ITER_TIMES << "ns" << std::endl;
    

    GR1e<64, 30>::MULT_METHOD_ = arith::KA_ONE_ITER;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 30> a = GR1e<64, 30>::random_element(ro);
        GR1e<64, 30> b = GR1e<64, 30>::random_element(ro);
        auto start9 = std::chrono::high_resolution_clock::now();
        GR1e<64, 30> c9 = a * b;                                          // Multiplication
        auto end9 = std::chrono::high_resolution_clock::now();
        total += end9 - start9;
        std::cout << "c9: " << c9.force_str() << std::endl;
    }
    std::cout << "KA one iter Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count()/ITER_TIMES << "ns" << std::endl;


    GR1e<64, 30>::MULT_METHOD_ = arith::KA_RECURSIVE;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 30> a = GR1e<64, 30>::random_element(ro);
        GR1e<64, 30> b = GR1e<64, 30>::random_element(ro);
        auto start10 = std::chrono::high_resolution_clock::now();
        GR1e<64, 30> c10 = a * b;                                          // Multiplication
        auto end10 = std::chrono::high_resolution_clock::now();
        total += end10 - start10;
        std::cout << "c10: " << c10.force_str() << std::endl;
    }
    std::cout << "KA recursive Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count()/ITER_TIMES << "ns" << std::endl;

    GR1e<64, 30>::MULT_METHOD_ = arith::KA_RECURSIVE_DUMMY;
    total = std::chrono::nanoseconds(0);
    for (int i = 0; i < ITER_TIMES; i++) {
        GR1e<64, 30> a = GR1e<64, 30>::random_element(ro);
        GR1e<64, 30> b = GR1e<64, 30>::random_element(ro);
        auto start11 = std::chrono::high_resolution_clock::now();
        GR1e<64, 30> c11 = a * b;                                          // Multiplication
        auto end11 = std::chrono::high_resolution_clock::now();
        total += end11 - start11;
        std::cout << "c11: " << c11.force_str() << std::endl;
    }
    std::cout << "KA recursive dummy Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count()/ITER_TIMES << "ns" << std::endl;
   
    // std::cout << f1.force_str() << std::endl;
    // std::cout << f2.force_str() << std::endl;
    // std::cout << f3.force_str() << std::endl;
    // std::cout << f4.force_str() << std::endl;

    return 0;

}
