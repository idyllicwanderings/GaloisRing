
#include "gring.h"
#include "lagrange.h"

#ifndef __SACRIFISING_BASED_CHECK_H
#define __SACRIFISING_BASED_CHECK_H

/**
 *  PARTY_NUM x multiplication_num
 */
template <typename Rs, typename Rl, int k, int s>
void sacrificing_check(const std::vector<std::vector<Rs>>& x_shares, 
                       const std::vector<std::vector<Rs>>& y_shares, 
                       const std::vector<std::vector<Rs>>& z_shares,
                       const std::vector<Rs>& ex_seq) 
{
    randomness::RO ro;
    ro.gen_random_bytes();

    /* =============== 1. Generate sharing of witness x, y, z =============== */
    using InShare = Rs;   //shares作用域。
    using ChkShare = Rl;

    constexpr uint64_t ds = Rs::get_d();
    constexpr uint64_t dl = Rl::get_d();
    static_assert(dl % ds == 0, "dl must be a multiple of ds");
    uint64_t m = x_shares[0].size();
    assert(y_shares[0].size() == m && z_shares[0].size() == m);
    int PARTY_NUM =  ex_seq.size();   // from 1 to n, not including R::zero()

    // std::vector<std::vector<InShare>> x_shares;
    // std::vector<std::vector<InShare>> y_shares;
    // std::vector<std::vector<InShare>> z_shares; 

    // for (int i = 0; i < n; i++) {
    //     x_shares.emplace_back(generate_sharing<InShare>(in_x[i], PARTY_NUM));
    //     y_shares.emplace_back(generate_sharing<InShare>(in_y[i], PARTY_NUM));
    //     z_shares.emplace_back(generate_sharing<InShare>(in_z[i], PARTY_NUM));
    // }

    /* ===================== 2. lift input shares to the check ring===================== */
    std::vector<std::vector<ChkShare>> lift_x_shares(PARTY_NUM), lift_y_shares(PARTY_NUM), lift_z_shares(PARTY_NUM);
    
    for (int i = 0; i < PARTY_NUM; i++) {
        for (int j = 0; j < m; j++) {
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
    std::vector<ChkShare> ex_seq_lifted;

    std::vector<Rl> a = Rl::random_vector(m, ro);
    for (int i = 0; i < m; i++) {
        c_shares.emplace_back(detail::elewise_product<ChkShare>(a, lift_y_shares[i]));
    }

    for (int i = 0; i < PARTY_NUM; i++) {
        ex_seq_lifted.emplace_back(liftGR<k + s, ds, dl>(ex_seq[i]));
    }

    for (int i = 0; i < m; i++) {
        a_shares.emplace_back(detail::generate_sharing<ChkShare>(a[i], ex_seq_lifted, PARTY_NUM));
    }

    /* ===================== 4. generate random epilson ================================ */
    // #ifdef GRtower
    //     // TODO: to support for GR towers，不好搞。。
    // #else
        
    // #endif
    Rl epsilon = extendGR<1 + s, k + s, dl>(GR1e<1 + s, dl>::random_element(ro));
    
    /* ===================== 5. recover check value ==================================== */
    std::vector<Rl> alpha(m);
    std::vector<std::vector<ChkShare>> alpha_shares;

    for (int i = 0; i < m; i++) {
        auto val = detail::elewise_subtract<Rl>(detail::dot_product<Rl>(lift_x_shares[i], epsilon), a_shares[i]);
        alpha_shares.emplace_back(val);
    }

    for (int i = 0; i < m; i++) { //reconstruct alpha
        alpha[i] = detail::interpolate<Rl>(alpha_shares[i], ex_seq_lifted, Rl::zero());
    }

    /* ===================== 6. zero check ============================================ */
    // TODO: modify this part, make sure the party_num aligns with the shares num,
    // TODO: modify the threshold t 

    // std::vector<std::vector<Rl>> opened_vals(PARTY_NUM);
    for (int i = 0; i < PARTY_NUM; i++) {
        std::vector<Rl> col(m);
        for (int j = 0; j < m; j++) {
            col.emplace_back(lift_z_shares[i][j] * epsilon \
                            - c_shares[i][j] - alpha_shares[i][j] * lift_y_shares[i][j]);
        } 
        Rl opened_val = detail::interpolate<Rl>(col, ex_seq_lifted, Rl::zero());
        assert(opened_val == Rl::zero());   
    }
}

#undef PARTY_NUM

#endif