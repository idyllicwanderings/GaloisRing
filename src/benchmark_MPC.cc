#include "../lib/gring.h"
#include "../lib/innprodcheck.h"
#include "../lib/compresscheck.h"
#include "../lib/saccheck.h"
#include "../lib/random.h"

#include <chrono>
#include <iostream>
#include <vector>
#include <string>

#ifndef k
#define k 32
#endif

#ifndef s
#define s 2
#endif


// N must be a power of 2
#ifndef n
#define n 1024
#endif

#ifndef PARTY_NUM
#define PARTY_NUM 15
#endif

#ifndef compress_factor
#define compress_factor 4
#endif

#ifndef t
#define t 1  // Remeber to set t to 0 for additive sharing
#endif

#ifndef TEST_CORRECTNESS
#define TEST_CORRECTNESS true
#endif


#define SAC_CHECK 1
#define INNER_CHECK 2
#define COMPRESS_CHECK 3

#ifndef CHECK_TYPE
// #define CHECK_TYPE SAC_CHECK
// #define CHECK_TYPE INNER_CHECK
#define CHECK_TYPE COMPRESS_CHECK
#endif

constexpr int d0 = 6;
constexpr int d1 = 4;


#define ADDITIVE_SHARING 0
#define THRESHOLD_SHARING 1
#ifndef SECRET_SHARING 
//# define SECRET_SHARING ADDITIVE_SHARING
#define SECRET_SHARING THRESHOLD_SHARING
#endif


#if CHECK_TYPE == COMPRESS_CHECK
using Rs = GR1e<k, d0>;
using Rl = GR1e<k, d1 * d0>;
#else
using Rs = GR1e<k + s, d0>;
using Rl = GR1e<k + s, d1 * d0>;
#endif


int main() {
    const int warmup = 0;
    const int repeat = 1;

    randomness::RO ro;
    ro.gen_random_bytes();

    std::vector<Rs> in_x = Rs::random_vector(n, ro);
    std::vector<Rs> in_y = Rs::random_vector(n, ro);
    std::vector<Rs> in_z;

    if constexpr (TEST_CORRECTNESS) {
        for (int i = 0; i < n; i++)
            in_z.push_back(in_x[i] * in_y[i]);
    } else {
        for (int i = 0; i < n - 1; i++)
            in_z.push_back(in_x[i] * in_y[i]);
        in_z.push_back(Rs::random_element(ro)); // break soundness
    }

    std::vector<std::vector<Rs>> x_shares, y_shares, z_shares;
    std::vector<Rs> ex_seq;

    if (SECRET_SHARING == ADDITIVE_SHARING) {
        std::cout << "Additive sharing" << std::endl;
        for (int i = 0; i < n; i++) {
            x_shares.emplace_back(detail::generate_additive_shares<Rs>(in_x[i], PARTY_NUM));
            y_shares.emplace_back(detail::generate_additive_shares<Rs>(in_y[i], PARTY_NUM));
            z_shares.emplace_back(detail::generate_additive_shares<Rs>(in_z[i], PARTY_NUM));
        }
    } 
    else if (SECRET_SHARING == THRESHOLD_SHARING) {
        std::cout << "Threshold sharing" << std::endl;
        std::vector<Rs> ex_zero = Rs::exceptional_seq(PARTY_NUM + 1);
        ex_seq = std::vector<Rs>(ex_zero.begin() + 1, ex_zero.end());
        for (int i = 0; i < n; i++) {
            x_shares.emplace_back(detail::generate_sharing<Rs>(in_x[i], ex_seq, t));
            y_shares.emplace_back(detail::generate_sharing<Rs>(in_y[i], ex_seq, t));
            z_shares.emplace_back(detail::generate_sharing<Rs>(in_z[i], ex_seq, t));
        }
    }
    // for (int i = 0; i < n; i++) {
    //     x_shares.emplace_back(detail::generate_sharing<Rs>(in_x[i], ex_seq, t));
    //     y_shares.emplace_back(detail::generate_sharing<Rs>(in_y[i], ex_seq, t));
    //     z_shares.emplace_back(detail::generate_sharing<Rs>(in_z[i], ex_seq, t));
    // }

    std::vector<std::vector<Rs>> x_shares_final, y_shares_final, z_shares_final;
    detail::transpose(x_shares, x_shares_final);
    detail::transpose(y_shares, y_shares_final);
    detail::transpose(z_shares, z_shares_final);

    std::cout << "Running check: " << CHECK_TYPE << ", n=" << n << ", t=" << t << std::endl;

    auto run_check = [&]() {
        // if (CHECK_TYPE == INNER_CHECK) {
        // inner_product_check<Rs, Rl, k, s, PARTY_NUM>(x_shares_final, y_shares_final, z_shares_final, ex_seq, t);
        // } else if (CHECK_TYPE == COMPRESS_CHECK) {
        compressed_multiplication_check<Rs, Rl, compress_factor, PARTY_NUM>(x_shares_final, y_shares_final, z_shares_final, ex_seq, t);
        // } else if (CHECK_TYPE == SAC_CHECK) {
        //    sacrificing_check<Rs, Rl, k, s, PARTY_NUM>(x_shares_final, y_shares_final, z_shares_final, ex_seq, t);
        // } else {
        //     throw std::runtime_error("Unknown CHECK_TYPE");
        // }
    };

    std::cout << "Warming up..." << std::endl;
    for (int i = 0; i < warmup; ++i) run_check();

    std::cout << "Benchmarking..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < repeat; ++i) run_check();
    auto end = std::chrono::high_resolution_clock::now();

    double avg_us = std::chrono::duration<double>(end - start).count() / repeat;
    std::cout << "Average time over " << repeat << " runs: " << avg_us << " s" << std::endl;

    return 0;
}
