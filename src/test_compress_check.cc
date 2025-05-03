#include "../lib/gring.h"
#include "../lib/compresscheck.h"
#include "../lib/random.h"


#include <chrono>
#include <iostream>
#include <vector>

// Default values can be overridden via -D flags in CMake
#ifndef PARTY_NUM
#define PARTY_NUM 6
#endif

#ifndef N
#define N 64
#endif

#ifndef TEST_CORRECTNESS
#define TEST_CORRECTNESS true
#endif

constexpr int WARMUP = 3;
constexpr int RUNS = 10;
constexpr int COMPRESS_FACTOR = 2;

using Rs = GR1e<32, 8>;
using Rl = GR1e<32, 16>;

std::vector<std::vector<Rs>> share_all(const std::vector<Rs>& vec, const std::vector<Rs>& ex_seq, int t) {
    std::vector<std::vector<Rs>> shares;
    for (const auto& el : vec)
        shares.emplace_back(detail::generate_sharing<Rs>(el, ex_seq, t));
    return shares;
}

double run_once(const std::vector<Rs>& ex_seq, randomness::RO& ro) {
    auto x = Rs::random_vector(N, ro);
    auto y = Rs::random_vector(N, ro);
    std::vector<Rs> z(N);

    if constexpr (TEST_CORRECTNESS) {
        for (int i = 0; i < N; ++i) z[i] = x[i] * y[i];
    } else {
        for (int i = 0; i < N; ++i) z[i] = Rs::random_element(ro);
    }

    const int t = PARTY_NUM - 1;
    auto x_shares = share_all(x, ex_seq, t);
    auto y_shares = share_all(y, ex_seq, t);
    auto z_shares = share_all(z, ex_seq, t);

    std::vector<std::vector<Rs>> x_final, y_final, z_final;
    detail::transpose(x_shares, x_final);
    detail::transpose(y_shares, y_final);
    detail::transpose(z_shares, z_final);

    auto start = std::chrono::high_resolution_clock::now();
    compressed_multiplication_check<Rs, Rl, COMPRESS_FACTOR>(x_final, y_final, z_final, ex_seq, t);
    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double, std::milli>(end - start).count();
}

int main() {
    randomness::RO ro;
    ro.gen_random_bytes();

    auto ex_zero = Rs::exceptional_seq(PARTY_NUM + 1);
    std::vector<Rs> ex_seq(ex_zero.begin() + 1, ex_zero.end());

    std::cout << "==> Running " << (TEST_CORRECTNESS ? "correctness" : "soundness") 
    << " test with N=" << N << ", PARTY_NUM=" << PARTY_NUM << "...\n";

    for (int i = 0; i < WARMUP; ++i) run_once(ex_seq, ro);

    double total_time = 0.0;
    for (int i = 0; i < RUNS; ++i) {
        double t = run_once(ex_seq, ro);
        std::cout << "  Run " << (i + 1) << ": " << t << " ms\n";
        total_time += t;
    }

    std::cout << "--> Average over " << RUNS << " runs: " << (total_time / RUNS) << " ms\n";
    return 0;
}


// int main () {
//     #define k 16
//     #define s 16  
//     #define n 8
//     #define COMPRESS_FACTOR 2

//     int PARTY_NUM = 6;
//     using Rs = GR1e<32, 8>;
//     using Rl = GR1e<32, 16>;

//     randomness::RO ro; 
//     ro.gen_random_bytes();
//     std::vector<Rs> ex_zero = Rs::exceptional_seq(PARTY_NUM + 1);
//     std::vector<Rs> ex_seq(ex_zero.begin() + 1, ex_zero.end());


//     // test detail::lagrange_coeff(z_full, alpha_end)
//     // have langrage coefficients as f(ex_seq) = x
//     // std::vector<Rs> test_x = Rs::random_vector(ex_seq.size(), ro);
//     // std::vector<Rs> coeffs = detail::lagrange_coeff<Rs>(ex_seq, test_x);

//     // Rs res = detail::horner_eval<Rs>(coeffs, ex_seq[0]);
//     // std::cout << "res: " << res.force_str() << std::endl;
//     // std::cout << "test_x[0]: " << test_x[0].force_str() << std::endl;

//     // assert(res == test_x[0]);
//     // std::cout << "lagrange_coeff test passed" << std::endl;


//     // for (int i = 0; i < PARTY_NUM; i++) {
//     //     std::cout << ex_seq[i].force_str() << std::endl;
//     // }

//     std::vector<Rs> in_x = Rs::random_vector(n, ro);
//     std::vector<Rs> in_y = Rs::random_vector(n, ro);
//     std::vector<Rs> in_z;

//     // generate random multiplications

//     // for (int i = 0; i < n; i++) {  //correctness check
//     //     in_z.push_back(in_x[i] * in_y[i]);
//     // }

//     for (int i = 0; i < n; i++) { //soundness check
//         in_z.push_back(Rs::random_element(ro));
//     }

//     // generate sharings      // multiplication_num x PARTY_NUM
//     std::vector<std::vector<Rs>> x_shares;
//     std::vector<std::vector<Rs>> y_shares;
//     std::vector<std::vector<Rs>> z_shares; 

//     int t = PARTY_NUM - 1;

//     for (int i = 0; i < n; i++) {
//         x_shares.emplace_back(detail::generate_sharing<Rs>(in_x[i], ex_seq, t));
//         y_shares.emplace_back(detail::generate_sharing<Rs>(in_y[i], ex_seq, t));
//         z_shares.emplace_back(detail::generate_sharing<Rs>(in_z[i], ex_seq, t));
//     }


//     // can you switch it back ???
//     std::vector<std::vector<Rs>> x_shares_final, y_shares_final, z_shares_final;
//     // now it is PARTY_NUM x multiplication_num
//     detail::transpose(x_shares, x_shares_final);
//     detail::transpose(y_shares, y_shares_final);
//     detail::transpose(z_shares, z_shares_final); 


//     std::cout << "prepared shares" << std::endl;

//     compressed_multiplication_check<Rs, Rl, 2>(x_shares_final, y_shares_final, z_shares_final, ex_seq, t);

//     return 1;
// }