
#include <gring.h>

namespace detail {

    std::array<bool, d> to_bits(std::int64_t value) {
        std::array<bool, d> bits;
        for (std::size_t i = 0; i < d; ++i) {
            bits[i] = (value >> i) & 1;
        }
        return bits;
    }


    template <typename R>
    R lagrange_l(const std::int64_t& lo, const std::int64_t& hi, const R& alpha_j, R x) {
        R res::one();
        R denom::one();
        for (std::int64_t i = lo; i < hi; i++) {
            R alpha_i = R::from_bits(to_bits(i));
            if (alpha_i == alpha_j) continue;
            res *= (x - alpha_i);
            denom *= (alpha_i - alpha_j);
        }
        return res * denom.inv();
    }

    template <typename R>
    R lagrange_l(const std::vector<R>& xcoords, R alpha_i, R x) {
        R res::one();
        R denom::one();
        for (const auto& alpha_j : xcoords) {
            if (alpha_j == alpha_i) continue;
            res *= (x - alpha_j);
            denom *= (alpha_i - alpha_j);
        }
        return res * denom.inv();
    }

    template <typename R>
    R interpolate(const std::vector<R>& ys, const R& x, uint64_t d_prod) {
        assert(ys.size() < (1ull << std::pow(2, d_prod)));
        R res::zero();
        for (std::size_t i = 0; i < ys.size(); i++) {
            res += ys[i] * detail::lagrange_l(0, ys.size(), i, x);
        }
        return res;
    }

}



template <int k>
void sacCheck(std::array<tuple<int, int>, k> x, y, z) {

    // TODO: generate sharing of witness x, y, z
    // TODO: langrange + reconstruct 
    std::vector<GRT<>> x_shares = langrange_compute(x);

    //lift x, y, z
    std::vector<GRT1e<k, d2>> lift_x;

    // generate random a and a*y = c
    a = GR<>::random_element();
    c = multiply(a, y);

    // random eplison
    epsilon = GR<>::random_element();

    // reconstruct alpha 
    alpha = reconstruct(multiply(epsilon, a) - lift_x);

    // zero check 
    multiply(epsilon, lift_z ) - c - multiply(alpha, lift_y) == 0;

}