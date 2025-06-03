
#include "gring.h"
#include "lagrange.h"
#include "random.h"

#ifndef __INNER_PRODUCT_CHECK_H
#define __INNER_PRODUCT_CHECK_H

template <typename Rs, typename Rl, int k, int s, int PARTY_NUM>
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
    uint64_t m = x_shares[0].size();
    // uint64_t PARTY_NUM = ex_seq.size();
    
    //std::cout << "defined parameters" << std::endl;

    /* ===================== 2. lift input shares to the check ring===================== */
    std::vector<std::vector<ChkShare>> lift_x_shares(PARTY_NUM), lift_y_shares(PARTY_NUM), lift_z_shares(PARTY_NUM);

    for (int i = 0; i < PARTY_NUM; i++) {
        for (int j = 0; j < m; j++) {
            #ifdef GRtower
                // TODO: to support for GR towers
            #else
            if constexpr (ds == dl) {
                lift_x_shares[i].emplace_back(x_shares[i][j]);
                lift_y_shares[i].emplace_back(y_shares[i][j]);
                lift_z_shares[i].emplace_back(z_shares[i][j]);
            } else {
                lift_x_shares[i].emplace_back(liftGR<k + s, ds, dl>(x_shares[i][j]));
                lift_y_shares[i].emplace_back(liftGR<k + s, ds, dl>(y_shares[i][j]));
                lift_z_shares[i].emplace_back(liftGR<k + s, ds, dl>(z_shares[i][j]));
            }
            #endif
        }
    }

    std::vector<ChkShare> ex_seq_lifted(PARTY_NUM);

    if (t != 0) { // threshold == 0 means additive sharing
        for (int i = 0; i < PARTY_NUM; i++) {
            if constexpr (ds == dl) {
                ex_seq_lifted[i] = ex_seq[i];
            } else {
                ex_seq_lifted[i] = liftGR<k + s, ds, dl>(ex_seq[i]);
            }
        }
    }

    // std::cout << "lifted shares" << std::endl;

    /* ===================== 3. generate random masking ================================ */

    std::vector<std::vector<ChkShare>> a_shares(PARTY_NUM);
    std::vector<ChkShare> c_shares(PARTY_NUM);

    std::vector<Rl> a = Rl::random_vector(m, ro);


    for (int i = 0; i < PARTY_NUM; i++) {
        c_shares[i] =  detail::dot_product<ChkShare>(a, lift_y_shares[i]);
    }

    for (int i = 0; i < m; i++) {
        std::vector<ChkShare> shares = (t==0) ? 
            detail::generate_additive_shares<ChkShare>(a[i], PARTY_NUM) :
            detail::generate_sharing<ChkShare>(a[i], ex_seq_lifted, t);
        for (int j = 0; j < PARTY_NUM; j++) {
            a_shares[j].emplace_back(shares[j]); 
        }
    }
    


    // std::cout << "generated random masking" << std::endl;

    /* ===================== 4. generate random epilson ================================ */
    #ifdef GRtower
        // TODO: to support for GR towers，不好搞。。
    #else
        auto eplison_n_unextended = GR1e<1 + s, dl>::random_vector(m, ro);
        
        std::vector<Rl> epsilon_n(m);
        for (int i = 0; i < m; i++) {
            epsilon_n[i] = extendGR<1 + s, k + s, dl>(eplison_n_unextended[i]);
        }
    #endif

    // std::cout << "generated random epsilon" << std::endl;
    
    /* ===================== 5. recover check value ==================================== */
    std::vector<std::vector<ChkShare>> alpha_shares(PARTY_NUM), alpha_shares_t;

    for (int i = 0; i < PARTY_NUM; i++) {
        alpha_shares[i] =  detail::elewise_subtract<Rl>(detail::elewise_product<Rl>(epsilon_n, 
                                                        lift_x_shares[i]), a_shares[i]);
    }
    
    detail::transpose(alpha_shares, alpha_shares_t);

    // std::cout << "transpose success" << std::endl;

    std::vector<Rl> alpha(m);
    for (int i = 0; i < m; i++) { //reconstruct alpha
        alpha[i] =  (t ==0)?  detail::reconstruct_additive_shares<Rl>(alpha_shares_t[i]) :
                    detail::interpolate<Rl>(alpha_shares_t[i], ex_seq_lifted, Rl::zero());
        //std::cout << "alpha[" << i << "] = " << std::endl;	
    }

    /* ===================== 6. zero check ============================================ */
    std::vector<Rl> col(PARTY_NUM);
    for (int i = 0; i < PARTY_NUM; i++) {
        Rl val =  detail::dot_product<Rl>(epsilon_n, lift_z_shares[i]) \
                    - c_shares[i] -  detail::dot_product<Rl>(alpha, lift_y_shares[i]);
        // std::cout << "interpolated check value: " << val.force_str() << std::endl;
        col[i] = val;
    }

    //std::cout << "reconstructed check value" << std::endl;
    
    Rl opened_val = (t == 0) ? 
        detail::reconstruct_additive_shares<Rl>(col) :
        detail::interpolate<Rl>(col, ex_seq_lifted, Rl::zero());
    //std::cout << "opened value: " << opened_val.force_str() << std::endl;
    // assert(opened_val == Rl::zero());

    //std::cout << "zero check success" << std::endl;

    
}

#endif