#include "../lib/gring.h"
#include "../lib/compresscheck.h"

#define MULT_NUM 16
#define PARTY_NUM 4 
#define COMPRESS_FACTOR 2



template <typename Rs, typename Rl>
void run_compress_check(const std::vector<Rs>& in_x, const std::vector<Rs>& in_y, const std::vector<Rs>& in_z)
{
    // ============================================================================================ //

    using InShare = Rs;
    std::vector<std::vector<InShare>> x_shares(n), y_shares(n), z_shares(n);
    int n = in_x.size();

    std::vector<Rs> alphas = Rs::exceptional_seq(PARTY_NUM + 2);
    alphas.erase(alphas.begin());

    for (int i = 0; i < n; i++) {
        x_shares.emplace_back(detail::generate_sharing<InShare>(in_x[i], PARTY_NUM));
        y_shares.emplace_back(detail::generate_sharing<InShare>(in_y[i], PARTY_NUM));
        z_shares.emplace_back(detail::generate_sharing<InShare>(in_z[i], PARTY_NUM));
    }

    // remeber to transpose!!!!

    std::vector<std::vector<InShare>> x_shares_t, y_shares_t, z_shares_t;
    detail::transpose(x_shares, x_shares_t);
    detail::transpose(y_shares, y_shares_t);
    detail::transpose(z_shares, z_shares_t);

    compressed_multiplication_check<Rs, Rl>(x_shares_t, y_shares_t, z_shares_t);

}

int main () {

    randomness::RO ro;
    ro.gen_random_bytes();

    using Rs = GR1e<32, 4>;
    using Rl = GR1e<32, 8>;
    std::vector<Rs> in_x = Rs::random_vector(MULT_NUM, ro);
    std::vector<Rs> in_y = Rs::random_vector(MULT_NUM, ro);
    std::vector<Rs> in_z = detail::elewise_product(in_x, in_y);

    run_compress_check<Rs, Rl>(in_x, in_y, in_z);

    return 1;
}