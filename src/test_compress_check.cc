#include "../lib/gring.h"
#include "../lib/compresscheck.h"
#include "../lib/random.h"


int main () {
    #define k 16
    #define s 16  
    #define n 2
    #define COMPRESS_FACTOR 2

    int PARTY_NUM = 5;
    using Rs = GR1e<32, 8>;
    using Rl = GR1e<32, 16>;

    randomness::RO ro; 
    ro.gen_random_bytes();
    std::vector<Rs> ex_zero = Rs::exceptional_seq(PARTY_NUM + 1);
    std::vector<Rs> ex_seq(ex_zero.begin() + 1, ex_zero.end());

    // for (int i = 0; i < PARTY_NUM; i++) {
    //     std::cout << ex_seq[i].force_str() << std::endl;
    // }

    std::vector<Rs> in_x = Rs::random_vector(n, ro);
    std::vector<Rs> in_y = Rs::random_vector(n, ro);
    std::vector<Rs> in_z;

    // generate random multiplications

    for (int i = 0; i < n; i++) {  //correctness check
        in_z.push_back(in_x[i] * in_y[i]);
    }

    // for (int i = 0; i < n - 1; i++) { //soundness check
    //     in_z.push_back(in_x[i] * in_y[i]);
    // }
    // in_z.push_back(Rs::random_element(ro));

    // generate sharings      // multiplication_num x PARTY_NUM
    std::vector<std::vector<Rs>> x_shares;
    std::vector<std::vector<Rs>> y_shares;
    std::vector<std::vector<Rs>> z_shares; 

    int t = PARTY_NUM - 1;

    for (int i = 0; i < n; i++) {
        x_shares.emplace_back(detail::generate_sharing<Rs>(in_x[i], ex_seq, t));
        y_shares.emplace_back(detail::generate_sharing<Rs>(in_y[i], ex_seq, t));
        z_shares.emplace_back(detail::generate_sharing<Rs>(in_z[i], ex_seq, t));
    }


    // can you switch it back ???
    std::vector<std::vector<Rs>> x_shares_final, y_shares_final, z_shares_final;
    // now it is PARTY_NUM x multiplication_num
    detail::transpose(x_shares, x_shares_final);
    detail::transpose(y_shares, y_shares_final);
    detail::transpose(z_shares, z_shares_final); 


    std::cout << "prepared shares" << std::endl;

    compressed_multiplication_check<Rs, Rl, 2>(x_shares_final, y_shares_final, z_shares_final, ex_seq, t);

    return 1;
}