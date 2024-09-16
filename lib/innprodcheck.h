
#include "gring.h"
#include "sacCheck.h"


#define PARTY_NUM 10 


template <typename Rs, typename Rl, int k, int s>
void inner_product_check(const std::vector<Rs>& in_x, const std::vector<Rs>& in_y, const std::vector<Rs>& in_z) {


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
    std::vector<ChkShare> c_share;

    std::vector<Rl> a = Rl::random_vector(n);
    // TODO: modify this
    std::vector<ChkShare> c = dot_product<Rl>(a, lift_y_shares);

    for (int i = 0; i < n; i++) {
        a_shares.emplace_back(generate_sharing<ChkShare>(a[i], PARTY_NUM));
        c_shares.emplace_back(generate_sharing<ChkShare>(c[i], PARTY_NUM));
    }

    /* ===================== 4. generate random epilson ================================ */
    #ifdef GRtower
        // TODO: to support for GR towers，不好搞。。
    #else
        std::vector<Rl> epsilon_n = extendGR<1 + s, k + s, dl>(GR1e<1 + s, dl>::random_vector<n>());
    #endif
    
    /* ===================== 5. recover check value ==================================== */
    std::vector<Rl> alpha(n);
    std::vector<std::vector<ChkShare>> alpha_shares;

    for (int i = 0; i < n; i++) {
        auto val = elewise_subtract<Rl>(dot_product<Rl>(epsilon_n, lift_x_shares[i]), a_shares[i]);
        alpha_shares.emplace_back(val);
    }

    for (int i = 0; i < n; i++) { //reconstruct alpha
        alpha[i] = interpolate(alpha_shares[i], Rl::zero());
    }

    /* ===================== 6. zero check ============================================ */

    std::vector<Rl> alpha_y_mult = dot_product<Rl>(alpha, lift_y_shares);
    std::vector<Rl> epsilon_z_mult = dot_product<Rl>(epsilon_n, lift_z_shares);

    for (int j = 0; j < PARTY_NUM; j++) {
        Rl opened_val = interpolate(eplison_z_mult[j] - c_share[j] - alpha_y_mult[j], Rl::zero());
        assert(opened_val == Rl::zero());
    }
    
}