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
    #define grt_k 32
    #define grt_d1 2
    #define grt_d2 0
    #define grt_d3 0
    #define grt_d4 0
    // grab random values for a, b, and c
    using GRTins = GRT1e<GRT1e<GR1e<grt_k, grt_d1>, grt_d2>, grt_d3>;
    GRTins a = GRTins::random_element();
    GRTins b = GRTins::random_element();

    auto start = nanos();
    // perform multiplication
    auto result = a * b;
    auto stop = nanos();
    auto duration = stop - start;

    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "b " << b.force_str() << std::endl;
    // std::cout << "a * b " << result.force_str() << std::endl;

    // std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
    //     << ") multiplication took " << duration << " nanoseconds" << std::endl;
    
    #undef grt_k
    #undef grt_d0
    #undef grt_d1
    #undef grt_d2
    return duration;
}

uint64_t GRT_add_benchmark() {
    #define grt_k 32
    #define grt_d1 2
    #define grt_d2 0
    #define grt_d3 0
    #define grt_d4 0
    using GRTins = GRT1e<GRT1e<GR1e<grt_k, grt_d1>, grt_d2>, grt_d3>;
    // grab random values for a, b, and c
    GRTins a = GRTins::random_element();
    GRTins b = GRTins::random_element();
    auto start = nanos();
    // perform addition
    auto result = a + b;
    auto stop = nanos();
    auto duration = stop - start;

    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "b " << b.force_str() << std::endl;
    // std::cout << "a + b " << result.force_str() << std::endl;

    // std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
    //     << ") addition took " << duration << " nanoseconds" << std::endl;
    
    #undef grt_k
    #undef grt_d1
    #undef grt_d2
    #undef grt_d3
    #undef grt_d4
    return duration;
}

uint64_t GRT_inv_benchmark() {
    #define grt_k 32
    #define grt_d1 2
    #define grt_d2 0
    #define grt_d3 0
    #define grt_d4 0
    // grab random values for a, b, and c
    using GRTins = GRT1e<GRT1e<GR1e<grt_k, grt_d1>, grt_d2>, grt_d3>;
    GRTins a = GRTins::random_element();
    auto start = nanos();
    auto result = a.inv();
    auto stop = nanos();
    auto duration = stop - start;
    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "a^-1 " << result.force_str() << std::endl;
    // std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
    //     << ") inverse took " << duration << " nanoseconds" << std::endl;
    #undef grt_k
    #undef grt_d1
    #undef grt_d2
    #undef grt_d3
    #undef grt_d4
    return duration;
}

uint64_t GR_mult_benchmark() {
    #define gr_k 6 
    #define gr_d1 12
    // grab random values for a, b, and c
    using GRins = GR1e<gr_k, gr_d1>;
    GRins a = GRins::random_element();
    GRins b = GRins::random_element();

    // perform multiplication
    auto start = nanos();
    auto result = a * b;
    auto stop = nanos();

    auto duration = stop - start;

    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "b " << b.force_str() << std::endl;
    // std::cout << "a * b " << result.force_str() << std::endl;

    // std::cout << "GR(" << k << ", " << d1 << ") multiplication took " << duration << " nanoseconds" << std::endl;
    
    #undef gr_k
    #undef gr_d1
    return duration;
}

uint64_t GR_add_benchmark() {
    #define gr_k 8
    #define gr_d1 12
    // grab random values for a, b, and c
    using GRins = GR1e<gr_k, gr_d1>;
    GRins a = GRins::random_element();
    GRins b = GRins::random_element();
    auto start = nanos();
    // perform addition
    auto result = a + b;
    auto stop = nanos();
    auto duration = stop - start;

    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "b " << b.force_str() << std::endl;
    // std::cout << "a + b " << result.force_str() << std::endl;

    // std::cout << "GR(" << k << ", " << d1 << ") addition took " << duration << " nanoseconds" << std::endl;
    
    #undef gr_k
    #undef gr_d1
    return duration;
}

uint64_t GR_inv_benchmark() {
    #define gr_k 8
    #define gr_d1 12
    // grab random values for a, b, and c
    using GRins = GR1e<gr_k, gr_d1>;
    GRins a = GRins::random_element();
    GRins b = GRins::random_element();

    auto start = nanos();
    // perform addition
    auto result = a.inv();
    auto stop = nanos();
    auto duration = stop - start;
    // std::cout << "a " << a.force_str() << std::endl;
    // std::cout << "a^-1 " << result.force_str() << std::endl;
    // std::cout << "GR(" << k << ", " << d1 << ") inverse took " << duration << " nanoseconds" << std::endl;
    #undef gr_k
    #undef gr_d1
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
        std::cout << "Average time for GRT addition: " << time << " microseconds\n" << std::endl;
        
        std::cout << " ================== gr towers: multiplication benchmark ===================== \n" << std::endl;
        time = 0;
        for (int i = 0; i < AVG_ITER; i++) {
            time += GRT_mult_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GRT multiplication: " << time << " nanoseconds\n" << std::endl;
        
        std::cout << " ================== gr towers: inverse benchmark ===================== \n" << std::endl;
        time = 0;
        for (int i = 0; i < AVG_ITER; i++) {
            time += GRT_inv_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GRT inverse: " << time << " nanoseconds\n" << std::endl;
        
        std::cout << " ================== gr towers benchmark ended =============================== \n" << std::endl;
    
    #endif

    #ifdef GR
        std::cout << " =========================== benchmark warm-up ============================= \n" << std::endl;
        for (int i = 0; i < 10; i++) {
            GR_add_benchmark();
        }
        std::cout << " ================== gr: addition benchmark =========================== \n" << std::endl;
        uint64_t time = 0;
        #define AVG_ITER 100
        for (int i = 0; i < AVG_ITER; i++) {
            time += GR_add_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GR addition: " << time << " nanoseconds" << std::endl;
        
        std::cout << " ================== gr: multiplication benchmark ===================== \n" << std::endl;
        time = 0;
        for (int i = 0; i < AVG_ITER; i++) {
            time += GR_mult_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GR multiplication: " << time << " nanoseconds" << std::endl;
        
        std::cout << " ================== gr: inverse benchmark ===================== \n" << std::endl;
        time = 0;
        for (int i = 0; i < AVG_ITER; i++) {
            time += GR_inv_benchmark() / AVG_ITER;
        }
        std::cout << "Average time for GR inverse: " << time << " nanoseconds" << std::endl;

        std::cout << " ================== gr benchmark ended =============================== \n" << std::endl;
    #endif
}

void run_ttl_benchmarks() {
    #ifdef GRT
        std::cout << " =========================== benchmark warm-up ============================= \n" << std::endl;
        for (int i = 0; i < 10; i++) {
            GRT_add_benchmark();
        }
        std::cout << " ================== gr towers: addition benchmark =========================== \n" << std::endl;
        #define TTL_ITER 100
        uint64_t start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GRT_add_benchmark();
        }
        uint64_t stop = micros();
        uint64_t time = (stop - start) / TTL_ITER;
        std::cout << "Average time for GRT addition: " << time << " microseconds\n" << std::endl;

        std::cout << " ================== gr towers: multiplication benchmark ===================== \n" << std::endl;
        start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GRT_mult_benchmark();
        }
        stop = micros();
        time = (stop - start) / TTL_ITER * 1000;
        std::cout << "Average time for GRT multiplication: " << time << " nanoseconds\n" << std::endl;

        std::cout << " ================== gr towers: inverse benchmark ===================== \n" << std::endl;
        start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GRT_inv_benchmark();
        }
        stop = micros();
        time = (stop - start) / TTL_ITER * 1000;
        std::cout << "Average time for GRT inverse: " << time << " nanoseconds\n" << std::endl;

    #endif

    #ifdef GR
        std::cout << " =========================== benchmark warm-up ============================= \n" << std::endl;
        for (int i = 0; i < 10; i++) {
            GR_add_benchmark();
        }
        std::cout << " ================== gr: addition benchmark =========================== \n" << std::endl;
        #define TTL_ITER 100
        uint64_t start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GR_add_benchmark();
        }
        uint64_t stop = micros();
        uint64_t time = (stop - start) / TTL_ITER;
        std::cout << "Average time for GR addition: " << time << " microseconds" << std::endl;

        std::cout << " ================== gr: multiplication benchmark ===================== \n" << std::endl;
        start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GR_mult_benchmark();
        }
        stop = micros();
        time = (stop - start) / TTL_ITER * 1000;
        std::cout << "Average time for GR multiplication: " << time << " nanoseconds" << std::endl;
    
        std::cout << " ================== gr: inverse benchmark ===================== \n" << std::endl;
        start = micros();
        for (int i = 0; i < TTL_ITER; i++) {
            GR_inv_benchmark();
        }
        stop = micros();
        time = (stop - start) / TTL_ITER * 1000;
        std::cout << "Average time for GR inverse: " << time << " nanoseconds" << std::endl;
    #endif

}

int main() {
    std::cout << " /////////////////////////// benchmarking by taking avg time of each run ///////////////////////////// \n\n" << std::endl;
    run_avg_benchmarks();
    std::cout << " /////////////////////////// benchmarking by taking total time of all runs /////////////////////////// \n\n" << std::endl;
    run_ttl_benchmarks();
    std::cout << " /////////////////////////// benchmarking ended ////////////////////////////////////////////////////// \n\n" << std::endl;
   
    return 1;
}
