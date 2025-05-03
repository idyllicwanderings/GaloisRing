#include "../lib/gring.h"
#include <chrono>
#include <iostream>
#include <functional>

#define AVG_ITER 10000
#define WARMUP_ITER 100

// Galois ring parameters
constexpr int gr_k = 64, gr_d1 = 8;
constexpr int lift_k = 32, lift_d1 = 3, lift_d2 = 24;

uint64_t nanos() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

template <typename Func>
double benchmark(const std::string& name, Func&& func) {
    for (int i = 0; i < WARMUP_ITER; ++i) func(); // warmup

    uint64_t total = 0;
    for (int i = 0; i < AVG_ITER; ++i)
        total += func();

    double avg = static_cast<double>(total) / AVG_ITER;
    std::cout << "Average time for " << name << ": " << avg << " ns\n" << std::endl;
    return avg;
}



uint64_t bench_mult() {
    using GRT = GR1e<gr_k, gr_d1>;
    randomness::RO ro; ro.gen_random_bytes();
    GRT a = GRT::random_element(ro), b = GRT::random_element(ro);
    auto start = nanos();
    auto c = a * b;
    return nanos() - start;
}

uint64_t bench_add() {
    using GRT = GR1e<gr_k, gr_d1>;
    randomness::RO ro; ro.gen_random_bytes();
    GRT a = GRT::random_element(ro), b = GRT::random_element(ro);
    auto start = nanos();
    auto c = a + b;
    return nanos() - start;
}

uint64_t bench_inv() {
    using GRT = GR1e<gr_k, gr_d1>;
    randomness::RO ro; ro.gen_random_bytes();
    GRT a = GRT::random_element(ro);
    auto start = nanos();
    auto c = a.inv();
    return nanos() - start;
}

uint64_t bench_lift() {
    using GRT = GR1e<lift_k, lift_d1>;
    randomness::RO ro; ro.gen_random_bytes();
    GRT a = GRT::random_element(ro);
    auto start = nanos();
    auto c = liftGR<lift_k, lift_d1, lift_d2>(a);
    return nanos() - start;
}

int main() {
    std::cout << "\n=========== Galois Ring Benchmarks ===========\n" << std::endl;
    benchmark("addition", bench_add);
    benchmark("multiplication", bench_mult);
    benchmark("inverse", bench_inv);
    benchmark("lift", bench_lift);
    std::cout << "==================== Done ====================\n" << std::endl;
    return 0;
}

// #define TTL_ITER 10000
// #define AVG_ITER 10000
// #define WARMUP_ITER 100


// uint64_t nanos()
// {
//     uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
//         std::chrono::high_resolution_clock::now().time_since_epoch()).count();
//     return ns; 
// }

// uint64_t micros()
// {
//     uint64_t ns = std::chrono::duration_cast<std::chrono::microseconds>(
//         std::chrono::high_resolution_clock::now().time_since_epoch()).count();
//     return ns; 
// }


// uint64_t GR_mult_benchmark() {
//     #define gr_k 64
//     #define gr_d1 8
//     #define gr_d2 0
//     #define gr_d3 0
//     #define gr_d4 0
//     // grab random values for a, b, and c

//     randomness::RO ro;
//     ro.gen_random_bytes(); 
//     using GRTins = GR1e<gr_k, gr_d1>;
//     GRTins a = GRTins::random_element(ro);
//     GRTins b = GRTins::random_element(ro);

//     auto start = nanos();
//     // perform multiplication
//     auto result = a * b;
//     auto stop = nanos();
//     auto duration = stop - start;

//     // std::cout << "a " << a.force_str() << std::endl;
//     // std::cout << "b " << b.force_str() << std::endl;
//     // std::cout << "a * b " << result.force_str() << std::endl;

//     // std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
//     //     << ") multiplication took " << duration << " nanoseconds" << std::endl;
    
//     #undef gr_k
//     #undef gr_d0
//     #undef gr_d1
//     #undef gr_d2
//     return duration;
// }

// uint64_t GR_add_benchmark() {
//     #define gr_k 64
//     #define gr_d1 8
//     #define gr_d2 0
//     #define gr_d3 0
//     #define gr_d4 0
//     using GRTins = GR1e<gr_k, gr_d1>;
//     // grab random values for a, b, and c
//     randomness::RO ro;
//     ro.gen_random_bytes(); 
//     GRTins a = GRTins::random_element(ro);
//     GRTins b = GRTins::random_element(ro);
//     auto start = nanos();
//     // perform addition
//     auto result = a + b;
//     auto stop = nanos();
//     auto duration = stop - start;

//     // std::cout << "a " << a.force_str() << std::endl;
//     // std::cout << "b " << b.force_str() << std::endl;
//     // std::cout << "a + b " << result.force_str() << std::endl;

//     // std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
//     //     << ") addition took " << duration << " nanoseconds" << std::endl;
    
//     #undef gr_k
//     #undef gr_d1
//     #undef gr_d2
//     #undef gr_d3
//     #undef gr_d4
//     return duration;
// }

// uint64_t GR_inv_benchmark() {
//     #define gr_k 64
//     #define gr_d1 8
//     #define gr_d2 0
//     #define gr_d3 0
//     #define gr_d4 0
//     // grab random values for a, b, and c
//     using GRTins = GR1e<gr_k, gr_d1>;
//     //TODO
//     randomness::RO ro;
//     ro.gen_random_bytes(); 
//     GRTins a = GRTins::random_element(ro);
//     auto start = nanos();
//     auto result = a.inv();
//     auto stop = nanos();
//     auto duration = stop - start;
//     // std::cout << "a " << a.force_str() << std::endl;
//     // std::cout << "a^-1 " << result.force_str() << std::endl;
//     // std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
//     //     << ") inverse took " << duration << " nanoseconds" << std::endl;
//     #undef gr_k
//     #undef gr_d1
//     #undef gr_d2
//     #undef gr_d3
//     #undef gr_d4
//     return duration;
// }

// uint64_t GR_lift_benchmark() {
//     #define lift_k 32
//     #define lift_d1 3
//     #define lift_d2 24
//     #define lift_d3 0
//     #define lift_d4 0
//     // grab random values for a, b, and c
//     using GRLiftins = GR1e<lift_k, lift_d1>;
//     randomness::RO ro;
//     ro.gen_random_bytes(); 
//     GRLiftins a = GRLiftins::random_element(ro);
//     auto start = nanos();
//     auto result = liftGR<lift_k, lift_d1, lift_d2>(a);
//     auto stop = nanos();
//     auto duration = stop - start;
//     // std::cout << "a " << a.force_str() << std::endl;
//     // std::cout << "a^ " << result.force_str() << std::endl;
//     // std::cout << "GR(GR(GR(" << k << ", " << d1 << "), " << d2 << "), " << d3
//     //     << ") lift took " << duration << " nanoseconds" << std::endl;
//     #undef gr_k
//     #undef gr_d1
//     #undef gr_d2
//     #undef gr_d3
//     #undef gr_d4
//     return duration;
// }

// void run_avg_benchmarks() {
//         //std::cout << " =========================== benchmark warm-up ============================= " << std::endl;
//         for (int i = 0; i < WARMUP_ITER; i++) {
//             GR_add_benchmark();
//         }
//         std::cout << " ================== avg time: addition benchmark =========================== " << std::endl;
//         double time = 0;
//         for (int i = 0; i < AVG_ITER; i++) {
//             time += GR_add_benchmark();
//         }

//         std::cout << "Average time for GRT addition: " << time / AVG_ITER << " nanoseconds\n" << std::endl;
        
//         std::cout << " ================== avg time: multiplication benchmark ===================== \n" << std::endl;
//         //std::cout << " =========================== benchmark warm-up ============================= " << std::endl;
//         for (int i = 0; i < WARMUP_ITER; i++) {
//             GR_mult_benchmark();
//         }
//         time = 0;
//         for (int i = 0; i < AVG_ITER; i++) {
//             time += GR_mult_benchmark();
//         }
//         std::cout << "Average time for GRT multiplication: " << time / AVG_ITER << " nanoseconds\n" << std::endl;
        
//         std::cout << " ================== gr towers: inverse benchmark ===================== \n" << std::endl;
//         std::cout << " =========================== benchmark warm-up ============================= " << std::endl;
//         for (int i = 0; i < WARMUP_ITER; i++) {
//             GR_inv_benchmark();
//         }
//         time = 0;
//         for (int i = 0; i < AVG_ITER; i++) {
//             time += GR_inv_benchmark();
//         }
//         std::cout << "Average time for GRT inverse: " << time / AVG_ITER << " nanoseconds\n" << std::endl;
        
//         std::cout << " ================== gr towers: lift benchmark ===================== \n" << std::endl;
//         std::cout << " =========================== benchmark warm-up ============================= " << std::endl;
//         for (int i = 0; i < WARMUP_ITER; i++) {
//             GR_lift_benchmark();
//         }
//         time = 0;
//         for (int i = 0; i < AVG_ITER; i++) {
//             time += GR_lift_benchmark() ;
//         }
//         std::cout << "Average time for GRT lift: " << time / AVG_ITER << " nanoseconds\n" << std::endl;

//         std::cout << " ================== avg time: gr benchmark ended =============================== \n" << std::endl;
// }

// void run_ttl_benchmarks() {
//     // std::cout << " ================== gr towers: addition benchmark =========================== \n" << std::endl;
//     // std::cout << " =========================== benchmark warm-up ============================= \n" << std::endl;
//     // for (int i = 0; i < WARMUP_ITER; i++) {
//     //     GR_add_benchmark();
//     // }
//     // uint64_t start = micros();
//     // for (int i = 0; i < TTL_ITER; i++) {
//     //     GR_add_benchmark();
//     // }
//     // uint64_t stop = micros();
//     // uint64_t time = (stop - start) / TTL_ITER;
//     // std::cout << "Average time for GRT addition: " << time << " microseconds\n" << std::endl;

//     // std::cout << " ================== gr towers: multiplication benchmark ===================== \n" << std::endl;
//     // std::cout << " =========================== benchmark warm-up ============================= \n" << std::endl;
//     // for (int i = 0; i < WARMUP_ITER; i++) {
//     //     GR_mult_benchmark();
//     // }
//     // start = micros();
//     // for (int i = 0; i < TTL_ITER; i++) {
//     //     GR_mult_benchmark();
//     // }
//     // stop = micros();
//     // time = (stop - start) / TTL_ITER;
//     // std::cout << "Average time for GRT multiplication: " << time << " microseconds\n" << std::endl;

//     // std::cout << " ================== gr towers: inverse benchmark ===================== \n" << std::endl;
//     // std::cout << " =========================== benchmark warm-up ============================= \n" << std::endl;
//     // for (int i = 0; i < WARMUP_ITER; i++) {
//     //     GR_inv_benchmark();
//     // }
//     // start = micros();
//     // for (int i = 0; i < TTL_ITER; i++) {
//     //     GR_inv_benchmark();
//     // }
//     // stop = micros();
//     // time = (stop - start) / TTL_ITER;
//     // std::cout << "Average time for GRT inverse: " << time << " microseconds\n" << std::endl;

//     // std::cout << " ================== gr towers: lift benchmark ===================== \n" << std::endl;
//     // std::cout << " =========================== benchmark warm-up ============================= \n" << std::endl;
//     // for (int i = 0; i < WARMUP_ITER; i++) {
//     //     GR_lift_benchmark();
//     // }
//     // start = micros();
//     // for (int i = 0; i < TTL_ITER; i++) {
//     //     GR_lift_benchmark();
//     // }
//     // stop = micros();
//     // time = (stop - start) / TTL_ITER;
//     // std::cout << "Average time for GRT lift: " << time << " microseconds\n" << std::endl;
    
//     // std::cout << " ================== gr benchmark ended =============================== \n" << std::endl;
// }

// int main() {
//     std::cout << " /////////////////////////// benchmarking by taking avg time of each run ///////////////////////////// \n\n" << std::endl;
//     run_avg_benchmarks();
//     // TODO: total time benchmarking could include other oprations like random_element...
//     // std::cout << " /////////////////////////// benchmarking by taking total time of all runs /////////////////////////// \n\n" << std::endl;
//     // run_ttl_benchmarks();
//     std::cout << " /////////////////////////// benchmarking ended ////////////////////////////////////////////////////// \n\n" << std::endl;
   
//     return 0;
// }

