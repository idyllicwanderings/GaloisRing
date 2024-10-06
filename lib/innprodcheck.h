
#include "gring.h"
#include "lagrange.h"
#include "random.h"

#ifndef __INNER_PRODUCT_CHECK_H
#define __INNER_PRODUCT_CHECK_H

template <typename Rs, typename Rl, int k, int s>
void inner_product_check(const std::vector<std::vector<Rs>>& x_shares, 
                         const std::vector<std::vector<Rs>>& y_shares, 
                         const std::vector<std::vector<Rs>>& z_shares,
                         const std::vector<Rs>& ex_seq,
                         int t)
{
    randomness::RO ro;
    ro.gen_random_bytes();

    /* =============== 1. Generate sharing of witness x, y, z =============== */
    using InShare = Rs;   //shares作用域。
    using ChkShare = Rl;

    constexpr uint64_t ds = Rs::get_d();
    constexpr uint64_t dl = Rl::get_d();
    static_assert(dl % ds == 0, "dl must be a multiple of ds");
    uint64_t PARTY_NUM = x_shares.size();
    assert(y_shares.size() == PARTY_NUM && z_shares.size() == PARTY_NUM);
    uint64_t n = ex_seq.size();
    
    std::cout << "defined parameters" << std::endl;

    /* ===================== 2. lift input shares to the check ring===================== */
    std::vector<std::vector<ChkShare>> lift_x_shares(PARTY_NUM), lift_y_shares(PARTY_NUM), lift_z_shares(PARTY_NUM);

    for (int i = 0; i < PARTY_NUM; i++) {
        for (int j = 0; j < n; j++) {
            #ifdef GRtower
                // TODO: to support for GR towers
            #else
                lift_x_shares[i].emplace_back(liftGR<k + s, ds, dl>(x_shares[i][j]));
                lift_y_shares[i].emplace_back(liftGR<k + s, ds, dl>(y_shares[i][j]));
                lift_z_shares[i].emplace_back(liftGR<k + s, ds, dl>(z_shares[i][j]));
            #endif
        }
    }

    std::vector<ChkShare> ex_seq_lifted(n);
    for (int i = 0; i < n; i++) {
        ex_seq_lifted[i] = (liftGR<k + s, ds, dl>(ex_seq[i]));
    }

    std::cout << "lifted shares" << std::endl;

    /* ===================== 3. generate random masking ================================ */

    std::vector<std::vector<ChkShare>> a_shares(PARTY_NUM);
    std::vector<ChkShare> c_shares(PARTY_NUM);

    std::vector<Rl> a = Rl::random_vector(n, ro);

    for (int i = 0; i < n; i++) {
        std::vector<ChkShare> shares = detail::generate_sharing<ChkShare>(a[i], ex_seq_lifted, t);
        for (int j = 0; j < PARTY_NUM; j++) {
            a_shares[j].emplace_back(shares[j]); 
        }
    }
    
    // TODO: modify this
    for (int i = 0; i < PARTY_NUM; i++) {
        c_shares[i] =  detail::dot_product<Rl>(a_shares[i], lift_y_shares[i]);
    }

    std::cout << "generated random masking" << std::endl;

    /* ===================== 4. generate random epilson ================================ */
    #ifdef GRtower
        // TODO: to support for GR towers，不好搞。。
    #else
        auto eplison_n_unextended = GR1e<1 + s, dl>::random_vector(n, ro);
        
        std::vector<Rl> epsilon_n(n);
        for (int i = 0; i < n; i++) {
            epsilon_n[i] = extendGR<1 + s, k + s, dl>(eplison_n_unextended[i]);
        }
    #endif

    std::cout << "generated random epsilon" << std::endl;
    
    /* ===================== 5. recover check value ==================================== */
    std::vector<Rl> alpha(n);
    std::vector<std::vector<ChkShare>> alpha_shares(PARTY_NUM), alpha_shares_t;

    for (int i = 0; i < PARTY_NUM; i++) {
        alpha_shares[i] =  detail::elewise_subtract<Rl>(detail::elewise_product<Rl>(epsilon_n, 
                                                        lift_x_shares[i]), a_shares[i]);
    }
    
    detail::transpose(alpha_shares, alpha_shares_t);

    std::cout << "transpose success" << std::endl;

    for (int i = 0; i < n; i++) { //reconstruct alpha
        alpha[i] =  detail::interpolate(alpha_shares_t[i], ex_seq_lifted, Rl::zero());
    }

    /* ===================== 6. zero check ============================================ */
    std::vector<Rl> col(PARTY_NUM);
    for (int i = 0; i < PARTY_NUM; i++) {
        Rl val =  detail::dot_product<Rl>(epsilon_n, lift_z_shares[i]) \
                    - c_shares[i] -  detail::dot_product<Rl>(alpha, lift_y_shares[i]);
        col[i] = val;
    }

    std::cout << "reconstructed check value" << std::endl;
    
    Rl opened_val = detail::interpolate(col, ex_seq_lifted, Rl::zero());
    std::cout << "opened value: " << opened_val.force_str() << std::endl;
    assert(opened_val == Rl::zero());

    std::cout << "zero check success" << std::endl;

    
}

#endif