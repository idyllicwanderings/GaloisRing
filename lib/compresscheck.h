#include "gring.h"
#include "lagrange.h"

#define PARTY_NUM 10 
#define COMPRESS_FACTOR 10
#define RAND_FLAG 1

Random::randomness ro;


template <typename R, int v, int l> 
std::vector<std::vector<R>> parse_polynomials (std::vector<R> x, int j)
{
    std::vector res;
    for (int i = 0; i < v; i++) {
        uint64_t len =  m / pow(v, j - 1);
        std::vector<R> tmp;
        for (int j = 0; j < len; j++) {
            tmp.emplace_back(x[i]);
        }
        res.emplace_back(tmp);
    }
    return res;
}

template<typename R>
void transpose(std::vector<std::vector<R>>& matrix) {
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = i + 1; j < matrix[i].size(); ++j) {
            std::swap(matrix[i][j], matrix[j][i]);
        }
    }
}

/**
 * v: compression factor, l: dimension, rand_flag
 * a:  第一维是compression factor ν,   第二维是多项式系数dimension ℓ
 */
template <typename R, int v, int l>
void compress_subroutine(std::vector<std::vector<R>>& x, std::vector<std::vector<R>>& y,std::vector<R>& z, 
                    std::vector<std::vector<R>>& x_out, std::vector<std::vector<R>>& y_out, std::vector<R>& z_out,
                    int RAND_FLAG)
{
    x_out.clear();
    y_out.clear();
    z_out.clear();

    assert(x.size() == v && y.size() == v && z.size() == v);
    assert(x[0].size() == l && y[0].size() == l);

    //transpose x, y to make it easier to compute
    transpose(x);
    transpose(y);

/*====================== 1. define two polynomials f, g ============================== */
    std::vector<R> alpha_w0 = R::exceptional_seq<2 * v + 2>(); // 不能用0！取2v+2个值
    std::vector<R> alpha(alpha_w0.begin() + 1, alpha_w0.end());   // only 2v + 1 values
    std::vector<R> alpha_v(alpha.begin(), alpha.begin() + v);  // extract first v elements
    std::vector<R> alpha_vp1(alpha.begin(), alpha.begin() + v + 1); // extract first v + 1 elements

    std::vector<std::vector<R>> f, g(l);

    if (RAND_FLAG) {
        R vs = R::random_element();
        R ws = R::random_element();
        for (int i = 0; i < l; i++) {
            f[i].emplace_back(detail::lagrange_coeff(std::vector<R>{x[i].begin(), x[i].end(), vs}, alpha_vp1));
            g[i].emplace_back(detail::lagrange_coeff(std::vector<R>{y[i].begin(), y[i].end(), ws}, alpha_vp1));
        }
    } 
    else {
        for (int i = 0; i < l; i++) {
            f[i].emplace_back(detail::lagrange_coeff(x[i], alpha_v));
            g[i].emplace_back(detail::lagrange_coeff(y[i], alpha_v));
        }
    }

/*====================== 2. inject z_i for the interpolation ========================= */
    std::vector<std::vector<R>> z_2nd_half; //z: [ν + 1, 2ν − 1]
    for (int j = v + 1; j < 2 * v; j++) {
        for (int i = 0; i < l; i++) {
            std::vector<R> z_j = detail::horner_eval(f[i], alpha[j]), detail::horner_eval(g[i], alpha[j]);
            z_2nd_half[i].emplace_back(z_j);
        }
    }   
    // z:  [v+1, 2v+1]
    if (RAND_FLAG) {
        for (int i = 2 * v; i <= 2 * v + 1; v++) {
            for (int i = 0; i < l; i++) {
                std::vector<R> z_j = detail::horner_eval(f[i], alpha[j]) * detail::horner_eval(g[i], alpha[j]);
                z_2nd_half[i].emplace_back(z_j);
            }
        }
    }

/*====================== 3. compute polynomial h ===================================== */
    std::vector<std::vector<R>> h;
    std::vector alpha_endm2(alpha.begin() + v - 1, alpha.end() - 2);
    std::vector alpha_end(alpha.begin() + v - 1, alpha.end());

    for (int i = 0; i < l; i++) {
        std::vector<R> z_full(z[i].begin(), z[i].end()); // z: [0, 2v-1]
        z_full.insert(z_full.end(), z_2nd_half[i].begin(), z_2nd_half[i].end());
        if (RAND_FLAG) {
            h[i].emplace_back(detail::lagrange_coeff(z_full, alpha_endm2));
        }
        else {
            h[i].emplace_back(detail::lagrange_coeff(z_full, alpha_end));
        }
    }

/*====================== 4. obtain challenge from exceptional sequence =============== */
    //TODO: get a byte, instantiate a random oracle for that
    uint64_t chal_bit = ro.gen_random_bytes(ro) % (v + 1) + v + 1;
    R chal_ele = alpha[chal_bit];
    // make sure alpha in [ v + 1, 2v + 1];

/*====================== 5. obtain output of the polynomials ========================= */
    for (int i = 0; i < l; i++) {
        x_out[i] = detail::horner_eval(f[i], chal_ele);
        y_out[i] = detail::horner_eval(g[i], chal_ele);
        z_out[i] = detail::horner_eval(h[i], chal_ele);
    }

}



/**
 * Rs is GR(2^k, d0), Rl is GR(2^k, d0* d1)
 * x_shares: x[0] is the first parties' shares
 */
template <typename Rs, typename Rl, int k>
void compressed_multiplication_check(const std::vector<std::vector<Rs>>& x_shares,
                                        const std::vector<std::vector<Rs>>& y_shares,
                                        const std::vector<std::vector<Rs>>& z_shares)   {
    using ChkShare = Rl;

    constexpr uint64_t ds = Rs::get_d();
    constexpr uint64_t dl = Rl::get_d();
    constexpr uint64_t k  = Rs::get_k();

    assert(x_shares.size() == PARTY_NUM && y_shares.size() == PARTY_NUM && z_shares.size() == PARTY_NUM);
    static_assert(dl % ds == 0, "dl must be a multiple of ds");

    uint64_t m = in_x.size();

/* ===================== 2. lift input shares to the check ring===================== */
    std::vector<std::vector<ChkShare>> lift_x_shares;
    std::vector<std::vector<ChkShare>> lift_y_shares;
    std::vector<std::vector<ChkShare>> lift_z_shares;

    for (int i = 0; i < PARTY_NUM;; i++) {
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
/* ===================== 3. generate inner product tuple =========================== */

/
    using MskRing = GR<2, dl>;
    std::vector<MskRing> yita;
    for (int i = 0; i < m; i++) {
        yita[i] = extendGR<2, k, dl>(MskRing::random_element());
    }

    std::vector<std::vector<ChkShare>> xj_shares;
    std::vector<std::vector<ChkShare>> yj_shares = lift_y_shares;
    std::vector<ChkShare> zj_shares;

    for (int j = 0; j < PARTY_NUM; j++) {
        std::vector<ChkShare> x0_share(m), z0_share;
        for (int i = 0; i < m; i++) {
            x0_share[i] = elewise_product<ChkShare>(lift_x_shares[i], yita[i]);
        }
        xj_shares.push_back(x0_share);
        z0_share = dot_product<ChkShare>(lift_z_shares[i], yita);
        zj_shares.push_back(z0_share);
    }


/* ===================== 4. execute the subroutine    ============================== */
    uint64_t logv_m = detail::log_base(COMPRESS_FACTOR, m);

    for (int j = 0; j < logv_m; j++) {   

        std::vector<std::vector<std::vector<ChkShare>>> a, b; // PARTY_NUM, COMPRESS_FACTOR, m
        std::vector<std::vector<ChkShare>> c;
        for (int i = 0; i < PARTY_NUM; i++) { 
            a_i = parse_polynomials<ChkShare, COMPRESS_FACTOR, l>(xj_shares[i], j);
            a.push_back(a_i);
            b_i = parse_polynomials<ChkShare, COMPRESS_FACTOR, l>(yj_shares[i], j);
            b.push_back(b_i);
            for (int k = 0; k < COMPRESS_FACTOR; k++) {
                c.emplace_back(detail::dot_product<ChkShare>(a_i[k], b_i[k]));
            }
        
            if (pow(COMPRESS_FACTOR, j) == m) {
                compress_subroutine(a[i], b[i], c[i], xj_shares[i], yj_shares[i], zj_shares[i], RAND_FLAG);
            }
            else {
                compress_subroutine(a[i], b[i], c[i], xj_shares[i], yj_shares[i], zj_shares[i], ~RAND_FLAG);
            }
        }
    }


/* ===================== 5. reconstruct the x values =============================== */
    // now xj_shares should be party_num个单元素向量
    std::vector<R> xjs;
    for (const auto& vec : xj_shares) {
        xjs.push_back(xj_shares[0]);  
    }
    R x_logv_m = detail::lagrange(xjs, Rl::zero());

/* ===================== 6. verify the result ====================================== */
    auto zero_ = elewise_subtract(dot_product<ChkShare>(yj_shares[logv_m], x_logv_m), zj_shares[logv_m]);
    
    for (int i = 0; i < PARTY_NUM;i ++) {
        assert(zero_[i] == ChkShare::zero());
    }

}





