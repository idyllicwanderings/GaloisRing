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
#define k 16
#endif

#ifndef s
#define s 16
#endif


// N must be a power of 2
#ifndef n
#define n 32
#endif

#ifndef PARTY_NUM
#define PARTY_NUM 10
#endif

#ifndef TEST_CORRECTNESS
#define TEST_CORRECTNESS true
#endif

#ifndef CHECK_TYPE
#define CHECK_TYPE "mul"
#endif

using Rs = GR1e<32, 8>;
using Rl = GR1e<32, 16>;

int main() {
    const int t = PARTY_NUM - 1;
    const int warmup = 10;
    const int repeat = 100;

    randomness::RO ro;
    ro.gen_random_bytes();
    std::vector<Rs> ex_zero = Rs::exceptional_seq(PARTY_NUM + 1);
    std::vector<Rs> ex_seq(ex_zero.begin() + 1, ex_zero.end());

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

    for (int i = 0; i < n; i++) {
        x_shares.emplace_back(detail::generate_sharing<Rs>(in_x[i], ex_seq, t));
        y_shares.emplace_back(detail::generate_sharing<Rs>(in_y[i], ex_seq, t));
        z_shares.emplace_back(detail::generate_sharing<Rs>(in_z[i], ex_seq, t));
    }

    std::vector<std::vector<Rs>> x_shares_final, y_shares_final, z_shares_final;
    detail::transpose(x_shares, x_shares_final);
    detail::transpose(y_shares, y_shares_final);
    detail::transpose(z_shares, z_shares_final);

    std::cout << "Running check: " << CHECK_TYPE << ", n=" << n << ", t=" << t << std::endl;

    auto run_check = [&]() {
        if (std::string(CHECK_TYPE) == "inner") {
            inner_product_check<Rs, Rl, k, s>(x_shares_final, y_shares_final, z_shares_final, ex_seq, t);
        } else if (std::string(CHECK_TYPE) == "mul") {
            constexpr int compress_factor = 2;
            compressed_multiplication_check<Rs, Rl, compress_factor>(x_shares_final, y_shares_final, z_shares_final, ex_seq, t);
        } else if (std::string(CHECK_TYPE) == "sac") {
            sacrificing_check<Rs, Rl, k, s>(x_shares_final, y_shares_final, z_shares_final, ex_seq, t);
        } else {
            throw std::runtime_error("Unknown CHECK_TYPE: " + std::string(CHECK_TYPE));
        }
    };

    std::cout << "Warming up..." << std::endl;
    for (int i = 0; i < warmup; ++i) run_check();

    std::cout << "Benchmarking..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < repeat; ++i) run_check();
    auto end = std::chrono::high_resolution_clock::now();

    double avg_us = std::chrono::duration<double, std::micro>(end - start).count() / repeat;
    std::cout << "Average time over " << repeat << " runs: " << avg_us << " us" << std::endl;

    return 0;
}
