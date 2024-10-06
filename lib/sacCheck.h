
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
                       const std::vector<Rs>& ex_seq,
                       int t) 
{
    randomness::RO ro;
    ro.gen_random_bytes();

    using InShare = Rs;   //shares作用域。
    using ChkShare = Rl;

    constexpr uint64_t ds = Rs::get_d();
    constexpr uint64_t dl = Rl::get_d();
    static_assert(dl % ds == 0, "dl must be a multiple of ds");
    uint64_t m = x_shares[0].size();
    assert(y_shares[0].size() == m && z_shares[0].size() == m);
    int PARTY_NUM =  ex_seq.size();   // from 1 to n, not including R::zero()

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

    std::cout << "lifted shares" << std::endl;

    /* ===================== 3. generate random masking ================================ */
    
    std::vector<std::vector<ChkShare>> a_shares(PARTY_NUM);
    std::vector<std::vector<ChkShare>> c_shares(PARTY_NUM);
    std::vector<ChkShare> ex_seq_lifted;

    std::vector<Rl> a = Rl::random_vector(m, ro);
    for (int i = 0; i < PARTY_NUM; i++) {
        c_shares[i] = detail::elewise_product<ChkShare>(a, lift_y_shares[i]);
    }


    for (int i = 0; i < PARTY_NUM; i++) {
        ex_seq_lifted.emplace_back(liftGR<k + s, ds, dl>(ex_seq[i]));
    }

    for (int i = 0; i < m; i++) {
        std::vector<ChkShare> shares = detail::generate_sharing<ChkShare>(a[i], ex_seq_lifted, t);
        for (int j = 0; j < PARTY_NUM; j++) {
            a_shares[j].emplace_back(shares[j]); 
        }
    }

    // for (int i = 0; i < m; i++) {
    //     a_shares.emplace_back(detail::generate_sharing<ChkShare>(a[i], ex_seq_lifted, t));
    // }

    std::cout << "generated random masking" << std::endl;


    /* ===================== 4. generate random epilson ================================ */
    // #ifdef GRtower
    //     // TODO: to support for GR towers，不好搞。。
    // #else
        
    // #endif
    Rl epsilon = extendGR<1 + s, k + s, dl>(GR1e<1 + s, dl>::random_element(ro));

    
    /* ===================== 5. recover check value ==================================== */
    std::vector<Rl> alpha(m);
    std::vector<std::vector<ChkShare>> alpha_shares(PARTY_NUM), alpha_shares_t;

    for (int i = 0; i < PARTY_NUM; i++) {
        alpha_shares[i] = detail::elewise_subtract<Rl>(detail::dot_product<Rl>(lift_x_shares[i], epsilon), a_shares[i]);
    }

    detail::transpose(alpha_shares, alpha_shares_t);

    std::cout << "transpose success" << std::endl;

    for (int i = 0; i < m; i++) { //reconstruct alpha
     
        alpha[i] = detail::interpolate<Rl>(alpha_shares_t[i], ex_seq_lifted, Rl::zero());
    }

    std::cout << "recovered check value" << std::endl;

    /* ===================== 6. zero check ============================================ */
    // TODO: modify this part, make sure the party_num aligns with the shares num,
    // TODO: modify the threshold t 

    // std::vector<std::vector<Rl>> opened_vals(PARTY_NUM);
    for (int j = 0; j < m; j++) {
        std::vector<Rl> col(PARTY_NUM);   
        for (int i = 0; i < PARTY_NUM; i++) {
            // 必须是alpha跟lift_y的点积，而不是alpha_shares跟lift_y_shares的点积（后者会变成degree-2n的多项式）
            // col[i] = Rl::zero();
            col[i] = lift_z_shares[i][j] * epsilon - c_shares[i][j] - alpha[j] * lift_y_shares[i][j];
        } 
        Rl opened_val = detail::interpolate<Rl>(col, ex_seq_lifted, Rl::zero());
        std::cout << "opened val: " << opened_val.force_str() << std::endl;
        assert(opened_val == Rl::zero());   
    }
    std::cout << "zero check passed" << std::endl;
}


#endif