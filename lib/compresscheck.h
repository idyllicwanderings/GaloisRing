#include "gring.h"
#include "lagrange.h"

#ifndef __COMPRESSED_MULTIPLICATION_CHECK_H
#define __COMPRESSED_MULTIPLICATION_CHECK_H


// #define PARTY_NUM 10 
// #define COMPRESS_V 10
#define RAND_FLAG 1


template <typename R, int v> 
std::vector<std::vector<R>> parse_polynomials(std::vector<R>& x, int j, int m)
{
    std::vector<std::vector<R>> res(v);
    for (int i = 0; i < v; i++) {
        uint64_t len =  m / pow(v, j - 1);
        std::vector<R> tmp;
        for (int j = 0; j < len; j++) {
            tmp.emplace_back(x[i]);
        }
        res[i] = tmp;
    }
    return res;
}


/**
 * v: compression factor, l: dimension, rand_flag
 * a:  第一维是compression factor ν,   第二维是多项式系数dimension ℓ
 */
template <typename R, int v>
void compress_subroutine(std::vector<std::vector<R>>& x_in, 
                         std::vector<std::vector<R>>& y_in,
                         std::vector<R>& z, 
                         std::vector<R>& x_out, 
                         std::vector<R>& y_out, 
                         R& z_out,
                         int rand_flag, 
                         randomness::RO& ro,
                         int l)
{
    x_out.clear();
    y_out.clear();
    z_out = R::zero();

    assert(x_in.size() == v && y_in.size() == v && z.size() == v);
    assert(x_in[0].size() == l && y_in[0].size() == l);

    //transpose x, y to make it easier to compute
    std::vector<std::vector<R>> x, y;
    detail::transpose<R>(x_in, x);
    detail::transpose<R>(y_in, y);

/*====================== 1. define two polynomials f, g ============================== */
    std::vector<R> alpha_w0 = R::exceptional_seq(2 * v + 2); // 不能用0！取2v+2个值
    std::vector<R> alpha(alpha_w0.begin() + 1, alpha_w0.end());   // only 2v + 1 values
    std::vector<R> alpha_v(alpha.begin(), alpha.begin() + v);  // extract first v elements
    std::vector<R> alpha_vp1(alpha.begin(), alpha.begin() + v + 1); // extract first v + 1 elements

    std::vector<std::vector<R>> f(l), g(l);

    if (rand_flag) {
        R vs = R::random_element(ro);
        R ws = R::random_element(ro);
        for (int i = 0; i < l; i++) {
            std::vector<R> tmp_vec1(x[i].begin(), x[i].end());
            tmp_vec1.push_back(vs);
            f[i] = detail::lagrange_coeff(tmp_vec1, alpha_vp1);
            std::vector<R> tmp_vec2(y[i].begin(), y[i].end());
            tmp_vec2.push_back(ws);
            g[i] = detail::lagrange_coeff(tmp_vec2, alpha_vp1);
        }
    } 
    else {
        for (int i = 0; i < l; i++) {
            f[i] = detail::lagrange_coeff(x[i], alpha_v);
            g[i] = detail::lagrange_coeff(y[i], alpha_v);
        }
    }

/*====================== 2. inject z_i for the interpolation ========================= */
    std::vector<R> z_2nd_half(v - 1); //z: [ν + 1, 2ν − 1]
    for (int j = v + 1; j < 2 * v; j++) {
        R z_j;
        for (int i = 0; i < l; i++) {
            z_j  += detail::horner_eval(f[i], alpha[j]) * detail::horner_eval(g[i], alpha[j]);
        }
    }   
    // z:  [v+1, 2v+1]
    if (rand_flag) {
        for (int j = 2 * v; j <= 2 * v + 1; j++) {
            R z_j;
            for (int i = 0; i < l; i++) {
                z_j += detail::horner_eval(f[i], alpha[j]) * detail::horner_eval(g[i], alpha[j]);
            }
            z_2nd_half.push_back(z_j);
        }
    }

/*====================== 3. compute polynomial h ===================================== */
    std::vector alpha_endm2(alpha.begin(), alpha.end() - 2);
    std::vector alpha_end(alpha.begin(), alpha.end());

    std::vector<R> z_full(z.begin(), z.end()); // z: [0, 2v-1]
    z_full.insert(z_full.end(), z_2nd_half.begin(), z_2nd_half.end());
    std::vector<R> h = (rand_flag) ? detail::lagrange_coeff(z_full, alpha_end) \
                                        : detail::lagrange_coeff(z_full, alpha_endm2);


/*====================== 4. obtain challenge from exceptional sequence =============== */
    //TODO: get a byte, instantiate a random oracle for that
    // v cannot be bigger than 2^8 - 1
    uint8_t chal_bit;
    ro.get_bytes(&chal_bit, 1);
    chal_bit = chal_bit % (v + 1) + v + 1;
    R chal_ele = alpha[chal_bit];
    // make sure alpha in [ v + 1, 2v + 1];

/*====================== 5. obtain output of the polynomials ========================= */
    for (int i = 0; i < l; i++) {
        x_out.emplace_back(detail::horner_eval(f[i], chal_ele));
        y_out.emplace_back(detail::horner_eval(g[i], chal_ele));
    }
    z_out = detail::horner_eval(h, chal_ele);

}



/**
 * Rs is GR(2^k, d0), Rl is GR(2^k, d0* d1)
 * x_shares: x[0] is the first parties' shares
 */
template <typename Rs, typename Rl, int COMPRESS_V>
void compressed_multiplication_check(const std::vector<std::vector<Rs>>& x_shares,
                                     const std::vector<std::vector<Rs>>& y_shares,
                                     const std::vector<std::vector<Rs>>& z_shares,
                                     const std::vector<Rs>& ex_seq,
                                     int t)   
{
    using ChkShare = Rl;

    constexpr uint64_t ds = Rs::get_d();
    constexpr uint64_t dl = Rl::get_d();
    constexpr uint64_t k  = Rs::get_k();
    uint64_t PARTY_NUM = ex_seq.size();   
    assert(x_shares.size() == PARTY_NUM && y_shares.size() == PARTY_NUM && z_shares.size() == PARTY_NUM);
    static_assert(dl % ds == 0, "dl must be a multiple of ds");

    uint64_t m = x_shares[0].size();

    randomness::RO ro;
    ro.gen_random_bytes();

    std::cout << "generated parameters" << std::endl;


/* ===================== 2. lift input shares to the check ring===================== */
    std::vector<std::vector<ChkShare>> lift_x_shares(PARTY_NUM), lift_y_shares(PARTY_NUM), lift_z_shares(PARTY_NUM);

    for (int i = 0; i < PARTY_NUM; i++) {
        for (int j = 0; j < m; j++) {
            #ifdef GRtower
                // TODO: to support for GR towers
            #else
                lift_x_shares[i].emplace_back(liftGR<k, ds, dl>(x_shares[i][j]));
                lift_y_shares[i].emplace_back(liftGR<k, ds, dl>(y_shares[i][j]));
                lift_z_shares[i].emplace_back(liftGR<k, ds, dl>(z_shares[i][j]));
            #endif
        }
    }

    std::vector<ChkShare> ex_seq_lifted(PARTY_NUM);
    for (int i = 0; i < PARTY_NUM; i++) {
        ex_seq_lifted[i] = liftGR<k, ds, dl>(ex_seq[i]);
    }

    std::cout << "lifted shares" << std::endl;

/* ===================== 3. generate inner product tuple =========================== */

    using MskRing = GR1e<2, dl>;

    std::vector<ChkShare> yita(m);
    for (int i = 0; i < m; i++) {
        yita[i] = extendGR<2, k, dl>(MskRing::random_element(ro));
    }

    std::vector<std::vector<ChkShare>> xj_shares(PARTY_NUM);
    std::vector<std::vector<ChkShare>> yj_shares = lift_y_shares;
    std::vector<ChkShare> zj_shares(PARTY_NUM);

    for (int i = 0; i < PARTY_NUM; i++) {
        xj_shares[i] = detail::elewise_product<ChkShare>(lift_x_shares[i], yita);
        zj_shares[i] = detail::dot_product<ChkShare>(lift_z_shares[i], yita);
    }

    std::cout << "generated inner product tuple" << std::endl;

/* ===================== 4. execute the subroutine    ============================== */
    uint64_t logv_m = detail::log_base(COMPRESS_V, m);

    for (int j = 0; j < logv_m; j++) {   
        std::vector<std::vector<std::vector<ChkShare>>> a(PARTY_NUM), b(PARTY_NUM); 
        // PARTY_NUM, COMPRESS_V, m
        std::vector<std::vector<ChkShare>> c(PARTY_NUM, std::vector<ChkShare>(COMPRESS_V));
        // PARTY_NUM, COMPRESS_V
        uint64_t l = m / pow(COMPRESS_V, j);

        for (int i = 0; i < PARTY_NUM; i++) { 
            std::vector<std::vector<ChkShare>> a_i = parse_polynomials<ChkShare, COMPRESS_V>(xj_shares[i], j, m);
            a[i] = a_i;
            std::vector<std::vector<ChkShare>> b_i = parse_polynomials<ChkShare, COMPRESS_V>(yj_shares[i], j, m);
            b[i] = b_i;

            for (int s = 0; s < COMPRESS_V; s++) {
                c[i][s] = (detail::dot_product<ChkShare>(a_i[s], b_i[s]));
            }
        
            if (l == 1) {
                compress_subroutine<ChkShare, COMPRESS_V>(a[i], b[i], c[i], \
                                    xj_shares[i], yj_shares[i], zj_shares[i], RAND_FLAG, ro, l);
            }
            else {
                compress_subroutine<ChkShare, COMPRESS_V>(a[i], b[i], c[i], \
                                    xj_shares[i], yj_shares[i], zj_shares[i], ~RAND_FLAG, ro, l);
            }
        }
    }


/* ===================== 5. reconstruct the x values =============================== */
    // now xj_shares should be party_num个单元素向量
    std::vector<Rl> xjs(PARTY_NUM);
    for (int i = 0; i < PARTY_NUM; i++) {
        xjs[i] = xj_shares[i][logv_m - 1];  
    }

    Rl x_logv_m = detail::interpolate(xjs, ex_seq_lifted, Rl::zero());

/* ===================== 6. verify the result ====================================== */
    std::vector<ChkShare> val(PARTY_NUM);
    for (int i = 0; i < PARTY_NUM; i++) {
        auto zero_ = yj_shares[i][logv_m - 1] * x_logv_m - zj_shares[i];
        val[i] = zero_;
    }

    Rl opened_val = detail::interpolate(val, ex_seq_lifted, Rl::zero());
    
    assert(opened_val == Rl::zero());

    std::cout << "zero check passed" << std::endl;

}



#undef PARTY_NUM
#undef COMPRESS_V
#undef RAND_FLAG

#endif