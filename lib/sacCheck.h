
#include "gring.h"

#define PARTY_NUM 10 

namespace detail {

    std::array<bool, d> to_bits(std::int64_t value) {
        std::array<bool, d> bits;
        for (std::size_t i = 0; i < d; ++i) {
            bits[i] = (value >> i) & 1;
        }
        return bits;
    }


    template <typename R>
    R lagrange_l(const std::int64_t& lo, const std::int64_t& hi, const R& alpha_i, R x) {
        R res::one();
        R denom::one();
        for (std::int64_t j = lo; j < hi; j++) {
            R alpha_j = R::from_bits(to_bits(j));
            if (alpha_j == alpha_i) continue;
            res *= (x - alpha_j);
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
    R interpolate(const std::vector<R>& ys, const R& x) {
        uint64_t d_prod = R::get_d();
        assert(ys.size() < (1ull << std::pow(2, d_prod)));
        R res;
        /// 0 作为secret, 1 到 ys.size() 作为shares
        for (std::size_t i = 0; i < ys.size(); i++) {
            res += ys[i] * detail::lagrange_l(1, ys.size() + 1, i + 1, x);
        }
        return res;
    }

    void interpolate_preprocess() {
        
    }

    template <typename R>
    std::vector<R> elewise_product(const std::vector<R>& a, const std::vector<R>& b) {
        std::vector<R> prod;
        for (int i = 0; i < a.size(); i++) {
            prod.push_back(a[i] * b[i]);
        }
        return prod;
    }

    template <typename R>
    std::vector<R> elewise_subtract(const std::vector<R>& a, const std::vector<R>& b) {
        std::vector<R> sub;
        for (int i = 0; i < a.size(); i++) {
            sub.push_back(a[i] - b[i]);
        }
        return sub;
    }
    
    template <typename R>
    std::vector<R> dot_product(const std::vector<R>& a, const R& b) {
        std::vector<R> res;
        for (int i = 0; i < a.size(); i++) {
            res.push_back(a[i] * b);
        }
        return res;
    }

    template <typename R>
    R horner_eval(const std::vector<R>& coeffs, const R& x) {
        R res = coeffs[0];
        for (int i = 1; i < coeffs.size(); i++) {
            res = res * x + coeffs[i];
        }
        return res;
    }

    template <typename R>
    std::vector<R> generate_sharing(const R& v, int t, int n) {
        std::vector<R> ys(n);
        // generate a polynomial of f(x) =(((a0x + a1)x + a2)x + a3)x + … )x + a_{t+1}( i.e. v).
        std::vector<R> coeffs(t + 1);
        for (int i = 0; i < t; i++) {
            coeffs.push_back(R::random_element());
        }
        coeffs.push_back(v);
        // compute 1 to n of the shares
        for (int i = 1; i <= n; i++) {
            ys[i] = (horner_eval(coeffs, i));
        }
        return ys;
    }




    /* =============== 1. Generate sharing of witness x, y, z =============== */
    void langrange_precompute(std::vector<R>& ys, uint64_t d_prod) {
    
    }

}




template <typename Rs, typename Rl, int k, int s>
void sacrificing_check(const std::vector<Rs>& in_x, const std::vector<Rs>& in_y, const std::vector<Rs>& in_z) {


    /* =============== 1. Generate sharing of witness x, y, z =============== */
    using InShare = Rs;   //shares作用域。
    using ChkShare = Rl;

    constexpr uint64_t ds = Rs::get_d();
    constexpr uint64_t dl = Rl::get_d();
    static_assert(dl % ds == 0, "dl must be a multiple of ds");
    uint64_t n = in_x.size();
    assert(in_y.size() == n && in_z.size() == n);

    std::vector<std::vector<InShare>> x_shares;
    std::vector<std::vector<InShare>> y_shares;
    std::vector<std::vector<InShare>> z_shares; 

    for (int i = 0; i < n; i++) {
        x_shares.emplace_back(generate_sharing<InShare>(in_x[i], PARTY_NUM));
        y_shares.emplace_back(generate_sharing<InShare>(in_y[i], PARTY_NUM));
        z_shares.emplace_back(generate_sharing<InShare>(in_z[i], PARTY_NUM));
    }

    /* ===================== 2. lift input shares to the check ring===================== */
    std::vector<std::vector<ChkShare>> lift_x_shares;
    std::vector<std::vector<ChkShare>> lift_y_shares;
    std::vector<std::vector<ChkShare>> lift_z_shares;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < PARTY_NUM; j++) {
            #ifdef GRtower
                // TODO: to support for GR towers
            #else
                lift_x_shares[i].emplace_back(liftGR<k + s, ds, dl>(x_shares[i][j]));
                lift_y_shares[i].emplace_back(liftGR<k + s, ds, dl>(y_shares[i][j]));
                lift_z_shares[i].emplace_back(liftGR<k + s, ds, dl>(z_shares[i][j]));
            #endif
        }
    }

    /* ===================== 3. generate random masking ================================ */
    
    std::vector<std::vector<ChkShare>> a_shares;
    std::vector<std::vector<ChkShare>> c_shares;

    std::vector<Rl> a = Rl::random_vector(n);
    std::vector<Rl> c = elewise_product<Rl>(a, lift_y);

    for (int i = 0; i < n; i++) {
        a_shares.emplace_back(generate_sharing<ChkShare>(a[i], PARTY_NUM));
        c_shares.emplace_back(generate_sharing<ChkShare>(c[i], PARTY_NUM));
    }

    /* ===================== 4. generate random epilson ================================ */
    #ifdef GRtower
        // TODO: to support for GR towers，不好搞。。
    #else
        Rl epsilon = extendGR<1 + s, k + s, dl>(GR1e<1 + s, dl>::random_element());
    #endif
    
    /* ===================== 5. recover check value ==================================== */
    std::vector<Rl> alpha(n);
    std::vector<std::vector<ChkShare>> alpha_shares;

    for (int i = 0; i < n; i++) {
        auto val = elewise_subtract<Rl>(dot_product<Rl>(epsilon, lift_x_shares[i]), a_shares[i]);
        alpha_shares.emplace_back(val);
    }

    for (int i = 0; i < n; i++) { //reconstruct alpha
        auto alpha[i] = interpolate(alpha_shares[i], Rl::zero());
    }

    /* ===================== 6. zero check ============================================ */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < PARTY_NUM; j++) {
            Rl opened_val = interpolate(epsilon * lift_z_shares[i][j] - c_shares[i][j] - alpha[i] * lift_y_shares[i][j], Rl::zero());
            assert(opened_val == Rl::zero());
        }
    }
}