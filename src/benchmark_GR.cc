#include "../lib/gring.h"

#include <chrono>
#include <iostream>

#define GRT

uint64_t nanos()
{
    uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    return ns; 
}

uint64_t micros()
{
    uint64_t ns = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    return ns; 
}



uint64_t GRT_mult_benchmark() {
    #define k 32
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
    return duration;
}

uint64_t GRT_add_benchmark() {
    #define k 32
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
    return duration;
}

uint64_t GR_mult_benchmark() {
    #define k 6 
    //uint8_t has some problems, fails for k <= 8, d1 = 12????????????
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

    // std::cout << "GR(" << k << ", " << d1 << ") multiplication took " << duration << " nanoseconds" << std::endl;
    
    #undef k
    #undef d0
    #undef d1
    #undef d2
    return duration;
}

uint64_t GR_add_benchmark() {
    #define k 8
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

    // std::cout << "GR(" << k << ", " << d1 << ") addition took " << duration << " nanoseconds" << std::endl;
    
    #undef k
    #undef d0
    #undef d1
    #undef d2
    return duration;
}

void run_avg_benchmarks() {
    #ifdef GRT
        std::cout << " =========================== benchmark warm-up ============================= " << std::endl;
        for (int i = 0; i < 10; i++) {
            GRT_add_benchmark();
        }
        std::cout << " ================== gr towers: addition benchmark =========================== " << std::endl;
        uint64_t time = 0;
        #define AVG_ITER 100
        for (int i = 0; i < AVG_ITER; i++) {
            time += GRT_add_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GRT addition: " << time << " nanoseconds" << std::endl;
        std::cout << " ================== gr towers: multiplication benchmark ===================== " << std::endl;
        time = 0;
        for (int i = 0; i < AVG_ITER; i++) {
            time += GRT_mult_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GRT multiplication: " << time << " nanoseconds" << std::endl;
        std::cout << " ================== gr towers benchmark ended =============================== " << std::endl;
    #endif

    #ifdef GR
        std::cout << " =========================== benchmark warm-up ============================= " << std::endl;
        for (int i = 0; i < 10; i++) {
            GR_add_benchmark();
        }
        std::cout << " ================== gr: addition benchmark =========================== " << std::endl;
        uint64_t time = 0;
        #define AVG_ITER 100
        for (int i = 0; i < AVG_ITER; i++) {
            time += GR_add_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GR addition: " << time << " nanoseconds" << std::endl;
        std::cout << " ================== gr: multiplication benchmark ===================== " << std::endl;
        time = 0;
        for (int i = 0; i < AVG_ITER; i++) {
            time += GR_mult_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GR multiplication: " << time << " nanoseconds" << std::endl;
        std::cout << " ================== gr benchmark ended =============================== " << std::endl;
    #endif
}

void run_ttl_benchmarks() {
    #ifdef GRT
        std::cout << " =========================== benchmark warm-up ============================= " << std::endl;
        for (int i = 0; i < 10; i++) {
            GRT_add_benchmark();
        }
        std::cout << " ================== gr towers: addition benchmark =========================== " << std::endl;
        #define TTL_ITER 100
        uint64_t start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GRT_add_benchmark();
        }
        uint64_t stop = micros();
        uint64_t time = (stop - start) / TTL_ITER * 1000;
        std::cout << "Average time for GRT addition: " << time << " nanoseconds" << std::endl;

        std::cout << " ================== gr towers: multiplication benchmark ===================== " << std::endl;
        start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GRT_mult_benchmark();
        }
        stop = micros();
        time = (stop - start) / TTL_ITER * 1000;
        std::cout << "Average time for GRT multiplication: " << time << " nanoseconds" << std::endl;
    #endif

    #ifdef GR
        std::cout << " =========================== benchmark warm-up ============================= " << std::endl;
        for (int i = 0; i < 10; i++) {
            GR_add_benchmark();
        }
        std::cout << " ================== gr: addition benchmark =========================== " << std::endl;
        #define TTL_ITER 100
        uint64_t start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GR_add_benchmark();
        }
        uint64_t stop = micros();
        uint64_t time = (stop - start) / TTL_ITER * 1000;
        std::cout << "Average time for GR addition: " << time << " nanoseconds" << std::endl;

        std::cout << " ================== gr: multiplication benchmark ===================== " << std::endl;
        start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GR_mult_benchmark();
        }
        stop = micros();
        time = (stop - start) / TTL_ITER * 1000;
        std::cout << "Average time for GR multiplication: " << time << " nanoseconds" << std::endl;
    #endif

}

int main() {
    std::cout << " /////////////////////////// benchmarking by taking avg time of each run ///////////////////////////// " << std::endl;
    run_avg_benchmarks();
    std::cout << " /////////////////////////// benchmarking by taking total time of all runs /////////////////////////// " << std::endl;
    run_ttl_benchmarks();
    std::cout << " /////////////////////////// benchmarking ended ////////////////////////////////////////////////////// " << std::endl;

   
    return 1;
}
