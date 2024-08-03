
#include "gring.h"

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

    void interpolate_preprocess() {
        
    }

    template <typename R, int n>
    std::array<R, n> elewise_product(const std::array<R, n>& a, const std::array<R, n>& b) {
        std::array<R, n> mult;
        for (int i = 0; i < n; i++) {
            mult[i] = a[i] * b[j];
        }
        return mult;
    }

    
    template <typename R, int n>
    std::array<R, n> dot_product(const std::array<R, n>& a, const R& b) {
        std::array<R, n> mult;
        for (int i = 0; i < n; i++) {
            mult[i] = a[i] * b;
        }
        return mult;
    }



    /* =============== 1. Generate sharing of witness x, y, z =============== */
    void langrange_precompute(std::vector<R>& ys, uint64_t d_prod) {
        std::vector<R> ys;
        interpolate<R>(ys, x, d_prod);

        std::array<R, k> s_x;
        std::array<R, k> s_y;
        std::array<R, k> s_z;
        for (int i = 0; i < k; i++) {
            s_x[i] = interpolate<R>(x, d_prod);
            s_y[i] = interpolate<R>(y, d_prod);
            s_z[i] = interpolate<R>(z, d_prod);
        }

    }

}




template <typename Rs, typename Rl, int k, int s>
void sacrificing_check(const std::array<Rs, n>& in_x, const std::array<Rs, n>& in_y, const std::array<Rs, n>& in_z) {

    // TODO: generate sharing of witness x, y, z
    // TODO: langrange + reconstruct 
    langrange_precompute<Rs>(ys, d_prod, in_x, in_y, in_z);

    //lift x, y, z
    uint64_t ds = Rs.get_d();
    uint64_t dl = Rl.get_d();

    /* ===================== 1. lift input shares to the check ring===================== */
    std::array<Rl, n> lift_x;
    std::array<Rl, n> lift_y;
    std::array<Rl, n> lift_z;

    for (int i = 0; i < n; i++) {
        lift_x[i] = liftGR<k + s, ds, dl>(shared_x[i]);
        lift_y[i] = liftGR<k + s, ds, dl>(shared_y[i]);
        lift_z[i] = liftGR<k + s, ds, dl>(shared_z[i]);
    }

    /* ===================== 2. generate random masking ================================ */
    
    std::array<Rl, n> a = Rl::random_vector<n>();
    shared_a = interpolate<>(a, d_prod);

    std::array<Rl, n> c = elewise_product<Rl, n>(shared_a, lift_y);
    shared_c = interpolate<>(c, d_prod);

    // random eplison
    /* ===================== 3. generate random epilson ================================ */
    GR1e<1 + s, dl> epsilon = GR1e<1 + s, dl>::random_element();
    //TODO: different range for epsilon and lift_z

    /* ===================== 4. recover check value ==================================== */
    auto alpha = reconstruct(dot_product<Rl, n>(epsilon, lift_x) - shared_a);

    /* ===================== 5. zero check ============================================ */
    assert(dot_product<Rl, n>(epsilon, lift_z) - shared_c - elewise_product(alpha, lift_y) == Rl::zero());

}