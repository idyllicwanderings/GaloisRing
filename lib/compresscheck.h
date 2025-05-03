#include "gring.h"
#include "lagrange.h"

#ifndef __COMPRESSED_MULTIPLICATION_CHECK_H
#define __COMPRESSED_MULTIPLICATION_CHECK_H

// #define PARTY_NUM 10
// #define COMPRESS_V 10
#define RAND_FLAG true

template <typename R, int v>
std::vector<std::vector<R>> parse_polynomials(std::vector<R> &x, int j, int m)
{
    std::vector<std::vector<R>> res(v);
    int len = m / pow(v, j);
    for (int i = 0; i < v; i++)
    {
        std::vector<R> tmp;
        for (int k = 0; k < len; k++)
        {
            tmp.emplace_back(x[i * len + k]);
        }
        res[i] = tmp;
    }
    return res;
}

/**
 * v: compression factor, l: dimension, rand_flag
 * a:  第一维是compression factor ν,   第二维是多项式系数dimension ℓ
 * TODO: z_i should be the inner product of reconstructed x_i and y_i.
 * So change the input vectors into a three-dimensional vector.
 */
template <typename R, int v>
void compress_subroutine(/* std::vector<std::vector<R>> &x_in, */
                         /* std::vector<std::vector<R>> &y_in,*/
                         /*  std::vector<R> &z, */
                         std::vector<R> &x_out,
                         std::vector<R> &y_out,
                         R &z_out,
                         bool rand_flag,
                         randomness::RO &ro,
                         int l,
                         std::vector<R> alpha, /* 2v+1 values*/
                         std::vector<R> &h,
                         std::vector<std::vector<R>> &f,
                         std::vector<std::vector<R>> &g)
{
    x_out.clear();
    y_out.clear();
    z_out = R::zero();

    // assert(x_in.size() == v && y_in.size() == v && z.size() == v);
    // assert(x_in[0].size() == l && y_in[0].size() == l);

    assert(alpha.size() == 2 * v + 1);

    // transpose x, y to make it easier to compute
    //  now is l x compress_v
    // std::vector<std::vector<R>> x, y;
    // detail::transpose<R>(x_in, x);
    // detail::transpose<R>(y_in, y);

    /*====================== 1. define two polynomials f, g ============================== */
    // std::vector<R> alpha_w0 = R::exceptional_seq(2 * v + 2);        // 不能用0！取2v+2个值
    // std::vector<R> alpha(alpha_w0.begin() + 1, alpha_w0.end());     // only 2v + 1 values
    // std::vector<R> alpha(alpha_seq.begin(), alpha_seq.begin() + 2 * v + 1); // only 2v + 1 values
    // std::vector<R> alpha_v(alpha.begin(), alpha.begin() + v);       // extract first v elements
    // std::vector<R> alpha_vp1(alpha.begin(), alpha.begin() + v + 1); // extract first v + 1 elements
    // std::vector alpha_m2(alpha.begin(), alpha.end() - 2);

    // std::vector<std::vector<R>> f(l), g(l);

    // if (rand_flag)
    // {
    //     for (int i = 0; i < l; i++)
    //     {
    //         R vs = R::random_element(ro);
    //         R ws = R::random_element(ro);
    //         std::vector<R> tmp_vec1(x[i].begin(), x[i].end());
    //         tmp_vec1.push_back(vs);
    //         f[i] = detail::lagrange_coeff(alpha_vp1, tmp_vec1);
    //         std::vector<R> tmp_vec2(y[i].begin(), y[i].end());
    //         tmp_vec2.push_back(ws);
    //         g[i] = detail::lagrange_coeff(alpha_vp1, tmp_vec2);
    //     }
    // }
    // else
    // {
    //     for (int i = 0; i < l; i++)
    //     {
    //         f[i] = detail::lagrange_coeff(alpha_v, x[i]);
    //         g[i] = detail::lagrange_coeff(alpha_v, y[i]);
    //     }
    // }

    /*====================== 2. inject z_i for the interpolation ========================= */
    // std::vector<R> z_2nd_half(v - 1); // z: [ν + 1, 2ν − 1]
    // for (int j = v; j < 2 * v - 1; j++)
    // {
    //     R z_j = R::zero();
    //     for (int i = 0; i < l; i++)
    //     {
    //         z_j += detail::horner_eval(f[i], alpha[j]) * detail::horner_eval(g[i], alpha[j]);
    //     }
    //     z_2nd_half[j - v] = z_j;
    // }
    // // z:  [v+1, 2v+1]
    // if (rand_flag)
    // {
    //     for (int j = 2 * v - 1; j <= 2 * v; j++)
    //     {
    //         R z_j = R::zero();
    //         for (int i = 0; i < l; i++)
    //         {
    //             z_j += detail::horner_eval(f[i], alpha[j]) * detail::horner_eval(g[i], alpha[j]);
    //         }
    //         z_2nd_half.push_back(z_j);
    //     }
    // }

    // /*====================== 3. compute polynomial h ===================================== */

    // std::vector<R> z_full(z.begin(), z.end());                         // z: [1, 2v-1]
    // z_full.insert(z_full.end(), z_2nd_half.begin(), z_2nd_half.end()); // z: [v+1, 2v+1 or 2v -1]

    // std::cout << "z.size() = " << z.size() << std::endl;
    // std::cout << "z_2nd_half.size() = " << z_2nd_half.size() << std::endl;
    // std::cout << "rand_flag = " << rand_flag << std::endl;

    // if (rand_flag) assert (z_full.size() == 2 * v + 1);
    // else assert (z_full.size() == 2 * v - 1);

    // std::cout << "LINE 124"<< std::endl;
    // std::vector<R> h = (rand_flag) ? detail::lagrange_coeff(alpha, z_full)
    //                                : detail::lagrange_coeff(alpha_m2, z_full);

    /*====================== 4. obtain challenge from exceptional sequence =============== */
    // TODO: get a byte, instantiate a random oracle for that
    //  v cannot be bigger than 2^8 - 1
    uint8_t chal_byte;
    ro.get_bytes(&chal_byte, 1);
    chal_byte = chal_byte % (v) + v;
    R chal_ele = alpha[chal_byte];
    // make sure alpha in [ v + 1, 2v + 1],注意这里index in alpha是[v, 2v]!!!

    /*====================== 5. obtain output of the polynomials ========================= */
    for (int i = 0; i < l; i++)
    {
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
void compressed_multiplication_check(const std::vector<std::vector<Rs>> &x_shares,
                                     const std::vector<std::vector<Rs>> &y_shares,
                                     const std::vector<std::vector<Rs>> &z_shares,
                                     const std::vector<Rs> &ex_seq,
                                     int threshold)
{
    using ChkShare = Rl;

    constexpr uint64_t ds = Rs::get_d();
    constexpr uint64_t dl = Rl::get_d();
    constexpr uint64_t k = Rs::get_k();
    uint64_t PARTY_NUM = ex_seq.size();
    assert(x_shares.size() == PARTY_NUM && y_shares.size() == PARTY_NUM && z_shares.size() == PARTY_NUM);
    static_assert(dl % ds == 0, "dl must be a multiple of ds");

    uint64_t m = x_shares[0].size();

    randomness::RO ro;
    ro.gen_random_bytes();

    // std::cout << "generated parameters" << std::endl;

    /* ===================== 2. lift input shares to the check ring===================== */
    std::vector<std::vector<ChkShare>> lift_x_shares(PARTY_NUM), lift_y_shares(PARTY_NUM), lift_z_shares(PARTY_NUM);

    for (int i = 0; i < PARTY_NUM; i++)
    {
        for (int j = 0; j < m; j++)
        {
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
    for (int i = 0; i < PARTY_NUM; i++)
    {
        ex_seq_lifted[i] = liftGR<k, ds, dl>(ex_seq[i]);
    }

    // std::cout << "lifted shares" << std::endl;

    /* ===================== 3. generate inner product tuple =========================== */

    using MskRing = GR1e<2, dl>;

    std::vector<ChkShare> yita(m);
    for (int i = 0; i < m; i++)
    {
        yita[i] = extendGR<2, k, dl>(MskRing::random_element(ro));
    }

    std::vector<std::vector<ChkShare>> xj_shares(PARTY_NUM);
    std::vector<std::vector<ChkShare>> yj_shares = lift_y_shares;
    std::vector<ChkShare> zj_shares(PARTY_NUM);

    for (int i = 0; i < PARTY_NUM; i++)
    {
        xj_shares[i] = detail::elewise_product<ChkShare>(lift_x_shares[i], yita);
        zj_shares[i] = detail::dot_product<ChkShare>(lift_z_shares[i], yita);
    }

    // std::cout << "generated inner product tuple" << std::endl;

    int DEBUGGG = 0;
    if (DEBUGGG == 1)
    {
        std::vector<std::vector<Rl>> xjs;
        detail::transpose<ChkShare>(xj_shares, xjs);
        std::vector<Rl> xj;
        for (int i = 0; i < m; i++)
        {
            xj.emplace_back(detail::interpolate(xjs[i], ex_seq_lifted, Rl::zero()));
        }
        std::vector<ChkShare> val(PARTY_NUM);
        for (int i = 0; i < PARTY_NUM; i++)
        {
            Rl zero_ = detail::dot_product<Rl>(yj_shares[i], xj) - zj_shares[i];
            val[i] = zero_;
        }

        Rl opened_val = detail::interpolate(val, ex_seq_lifted, Rl::zero());

        // std::cout << "1st round  opened_val = " << opened_val.force_str() << std::endl;

        assert(opened_val == Rl::zero());

        // std::cout << "round zero check passed" << std::endl;
        // std::cout << "the first check passed" << std::endl;
    }

    /* ===================== 4. execute the subroutine    ============================== */
    uint64_t logv_m = detail::log_base(COMPRESS_V, m);

    // std::cout << "logv_m = " << logv_m << std::endl;  

    for (int j = 1; j <= logv_m; j++)
    {
        std::vector<std::vector<std::vector<ChkShare>>> a_shares(PARTY_NUM), b_shares(PARTY_NUM);
        // PARTY_NUM, COMPRESS_V, l
        std::vector<std::vector<ChkShare>> c_shares(PARTY_NUM, std::vector<ChkShare>(COMPRESS_V));
        std::vector<ChkShare> c(COMPRESS_V);
        // PARTY_NUM, COMPRESS_V
        uint64_t l = m / pow(COMPRESS_V, j);

        for (int i = 0; i < PARTY_NUM; i++)
        {
            // for (int s = 0; s < m; s++) {
            //     std::cout << "xj_shares[" << i << "]" << "[" << s << "] = "
            //     << xj_shares[i][s].force_str() << std::endl;
            // }

            std::vector<std::vector<ChkShare>> a_i = parse_polynomials<ChkShare, COMPRESS_V>(xj_shares[i], j, m);
            a_shares[i] = a_i;

            // for (int s = 0; s < COMPRESS_V; s++) {
            //     for (int t = 0; t < l; t++) {
            //         std::cout << "a_i[" << s << "]" << "[" << t << "] = "
            //          << a_i[s][t].force_str() << std::endl;
            //     }
            // }

            std::vector<std::vector<ChkShare>> b_i = parse_polynomials<ChkShare, COMPRESS_V>(yj_shares[i], j, m);
            b_shares[i] = b_i;
        }

        std::vector<std::vector<ChkShare>> a(COMPRESS_V), b(COMPRESS_V); // compress v * l
        for (int s = 0; s < COMPRESS_V; s++)
        {
            std::vector<ChkShare> at(l), bt(l);
            for (int t = 0; t < l; t++)
            {
                std::vector<ChkShare> ap_col(PARTY_NUM), bp_col(PARTY_NUM);
                for (int p = 0; p < PARTY_NUM; p++)
                {
                    ap_col[p] = a_shares[p][s][t];
                    bp_col[p] = b_shares[p][s][t];
                }
                at[t] = detail::interpolate<ChkShare>(ap_col, ex_seq_lifted, ChkShare::zero());
                bt[t] = detail::interpolate<ChkShare>(bp_col, ex_seq_lifted, ChkShare::zero());
            }
            a[s] = at; // save reconstructed a, b , no need for sharing of c
            b[s] = bt;
            c[s] = detail::dot_product<ChkShare>(at, bt);
            // std::vector<ChkShare> shares = detail::generate_sharing<ChkShare>(c, ex_seq_lifted, threshold);
            // for (int p = 0; p < PARTY_NUM; p++) {
            //     c_shares[p][s] = shares[p];
            // }
        }

        ChkShare z = detail::interpolate<ChkShare>(zj_shares, ex_seq_lifted, ChkShare::zero());
        // std::cout << "z = " << z.force_str() << std::endl;
        ChkShare sum_c = ChkShare::zero();
        for (int s = 0; s < COMPRESS_V - 1; s++)
        {
            sum_c += c[s];
        }
        // std::cout << "sum_c = " << sum_c.force_str() << std::endl;
        c[COMPRESS_V - 1] = z - sum_c;
        // std::cout << "c[COMPRESS_V - 1] = " << c[COMPRESS_V - 1].force_str() << std::endl;
        
        ChkShare amb = ChkShare::zero();
        // for (int s = 0; s < COMPRESS_V; s++) {
            // amb += detail::dot_product<ChkShare>(a[s], b[s]);
        // }
        amb = detail::dot_product<ChkShare>(a[COMPRESS_V - 1], b[COMPRESS_V - 1]);
        // std::cout << "amb = " << amb.force_str() << std::endl;

        // compute z_i from hint oracle
        std::vector<std::vector<ChkShare>> a_t, b_t; // l * v

        detail::transpose<ChkShare>(a, a_t);
        detail::transpose<ChkShare>(b, b_t);


        // extract the first 2v+2 exceptional sequence
        int v = COMPRESS_V;
        std::vector<ChkShare> alpha(ex_seq_lifted.begin(), ex_seq_lifted.begin() + 2 * v + 1); // only 2v + 1 values
        std::vector<ChkShare> alpha_v(alpha.begin(), alpha.begin() + v);                       // extract first v elements
        std::vector<ChkShare> alpha_vp1(alpha.begin(), alpha.begin() + v + 1);                 // extract first v + 1 elements
        std::vector<ChkShare> alpha_m2(alpha.begin(), alpha.end() - 2);

        std::vector<std::vector<ChkShare>> f(l), g(l);

        // compute polynomials f, g
        if (l == 1)
        { // i.e. RAND_FLAG = 1
            ChkShare vs = ChkShare::random_element(ro);
            ChkShare ws = ChkShare::random_element(ro);
            // std::vector<ChkShare> tmp_vec1(a_t[0].begin(), a_t[0].end());
            // tmp_vec1.push_back(vs);
            // f[0] = detail::lagrange_coeff(alpha_vp1, tmp_vec1);
            // std::vector<ChkShare> tmp_vec2(b_t[0].begin(), b_t[0].end());
            // tmp_vec2.push_back(ws);
            // g[0] = detail::lagrange_coeff(alpha_vp1, tmp_vec2);
            a_t[0].push_back(vs);
            b_t[0].push_back(ws);
            f[0] = detail::lagrange_coeff(alpha_vp1, a_t[0]);
            g[0] = detail::lagrange_coeff(alpha_vp1, b_t[0]);
        }
        else
        {
            for (int i = 0; i < l; i++)
            {
                f[i] = detail::lagrange_coeff(alpha_v, a_t[i]);
                g[i] = detail::lagrange_coeff(alpha_v, b_t[i]);
            }
        }

        // std::cout << "generated polynomials f, g" << std::endl;

        // generate shares for f, g
        std::vector<std::vector<std::vector<ChkShare>>> f_shares(PARTY_NUM, std::vector<std::vector<ChkShare>>(l, std::vector<ChkShare>(f[0].size())));
        std::vector<std::vector<std::vector<ChkShare>>> g_shares(PARTY_NUM, std::vector<std::vector<ChkShare>>(l, std::vector<ChkShare>(f[0].size())));
        for (int s = 0; s < l; s++)
        {
            for (int t = 0; t < f[0].size(); t++)
            {
                std::vector<ChkShare> fshare = detail::generate_sharing<ChkShare>(f[s][t], ex_seq_lifted, threshold);
                std::vector<ChkShare> gshare = detail::generate_sharing<ChkShare>(g[s][t], ex_seq_lifted, threshold);
                for (int i = 0; i < PARTY_NUM; i++)
                {
                    f_shares[i][s][t] = fshare[i];
                    g_shares[i][s][t] = gshare[i];
                }
            }
        }

        // std::cout << "generated shares for f, g" << std::endl;

        std::vector<ChkShare> z_2nd_half(v - 1); // z: [ν + 1, 2ν − 1]
        for (int s = v; s < 2 * v - 1; s++)
        {
            ChkShare z_s = ChkShare::zero();
            for (int i = 0; i < l; i++)
            {
                z_s += detail::horner_eval(f[i], alpha[s]) * detail::horner_eval(g[i], alpha[s]);
            }
            z_2nd_half[s - v] = z_s;
        }
        // z:  [v+1, 2v+1]
        if (l == 1)
        { // i.e. RAND_FLAG = 1
            for (int s = 2 * v - 1; s <= 2 * v; s++)
            {
                ChkShare z_s = detail::horner_eval(f[0], alpha[s]) * detail::horner_eval(g[0], alpha[s]);
                z_2nd_half.push_back(z_s);
            }
        }

        // std::cout << "generated z_2nd_half" << std::endl;

        /*====================== 3. compute polynomial h ===================================== */

        std::vector<ChkShare> z_full(c.begin(), c.end());                  // z: [1, 2v-1]
        z_full.insert(z_full.end(), z_2nd_half.begin(), z_2nd_half.end()); // z: [v+1, 2v+1 or 2v -1]

        if (l == 1)  
            assert(z_full.size() == 2 * v + 1);
        else
            assert(z_full.size() == 2 * v - 1);

        std::vector<ChkShare> h = ((l == 1) ? detail::lagrange_coeff(alpha, z_full) : detail::lagrange_coeff(alpha_m2, z_full));

        // share the polynomial h
        std::vector<std::vector<ChkShare>> h_shares(PARTY_NUM);
        for (int s = 0; s < h.size(); s++)
        {
            std::vector<ChkShare> hshare = detail::generate_sharing<ChkShare>(h[s], ex_seq_lifted, threshold);
            for (int i = 0; i < PARTY_NUM; i++)
            {
                h_shares[i].emplace_back(hshare[i]);
            }
        }

        // std::cout << "generated shares for polynomial h" << std::endl;

        // NOTE: the challenge should be the same!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        /*====================== 4. obtain challenge from exceptional sequence =============== */
        // TODO: get a byte, instantiate a random oracle for that
        //  v cannot be bigger than 2^8 - 1
        uint8_t chal_byte;
        ro.get_bytes(&chal_byte, 1);
        chal_byte = chal_byte % (v) + v;
        // challenge should be selected from [v + 1, 2^d - 1]
        
        ChkShare chal_ele = alpha[chal_byte]; // make sure alpha in [ v + 1, 2v + 1],注意这里index in alpha是[v, 2v]!!!
        //std::cout << "------------------------------------" << std::endl;
        //std::cout << "chal_byte = " << static_cast<uint32_t>(chal_byte) << std::endl;
        //std::cout << "------------------------------------" << std::endl;

        /*====================== 5. obtain output of the polynomials ========================= */
        for (int i = 0; i < PARTY_NUM; i++)
        {
            xj_shares[i].clear();
            yj_shares[i].clear();
            for (int s = 0; s < l; s++)
            {
                xj_shares[i].emplace_back(detail::horner_eval(f_shares[i][s], chal_ele));
                yj_shares[i].emplace_back(detail::horner_eval(g_shares[i][s], chal_ele));
            }
            zj_shares[i] = detail::horner_eval(h_shares[i], chal_ele);
        }

        // std::cout << "round " << j << " finished" << std::endl;

        // for (int i = 0; i < PARTY_NUM; i++) {
        //     if (l == 1) {

        //         std::cout << "HERE COMES final round" << std::endl;
        //         compress_subroutine<ChkShare, COMPRESS_V>( /*a_shares[i], b_shares[i],*/
        //                                                     c_shares[i],
        //                                                     xj_shares[i], yj_shares[i], zj_shares[i], RAND_FLAG, ro, l,
        //                                                     /*ex_seq_lifted,*/ alpha,
        //                                                     z_2nd_half, f_shares[i], g_shares[i]);
        //     }
        //     else {
        //         compress_subroutine<ChkShare, COMPRESS_V>(/*a_shares[i], b_shares[i], */
        //                                                     c_shares[i],
        //                                                     xj_shares[i], yj_shares[i], zj_shares[i], !RAND_FLAG, ro, l,
        //                                                     /*ex_seq_lifted,*/ alpha,
        //                                                     z_2nd_half, f_shares[i], g_shares[i]);
        //         // *** test each round ************************************
        //     }
        // }

        if (DEBUGGG != 1)
        {
            std::vector<std::vector<Rl>> xjs;
            detail::transpose<ChkShare>(xj_shares, xjs);
            std::vector<Rl> xj;
            for (int i = 0; i < l; i++)
            {
                xj.emplace_back(detail::interpolate(xjs[i], ex_seq_lifted, Rl::zero()));
            }
            std::vector<ChkShare> val(PARTY_NUM);
            for (int i = 0; i < PARTY_NUM; i++)
            {
                Rl zero_ = detail::dot_product<Rl>(yj_shares[i], xj) - zj_shares[i];
                val[i] = zero_;
            }

            Rl opened_val = detail::interpolate(val, ex_seq_lifted, Rl::zero());

            // std::cout << "round " << j << " opened_val = " << opened_val.force_str() << std::endl;

            assert(opened_val == Rl::zero());

            // std::cout << "round" << j << " zero check passed" << std::endl;
        }
    }

    /* ===================== 5. reconstruct the x values =============================== */
    // now xj_shares should be party_num个单元素向量
    std::vector<Rl> xjs(PARTY_NUM);
    for (int i = 0; i < PARTY_NUM; i++)
    {
        xjs[i] = xj_shares[i][0];
        // std::cout << "xj_shares[" << i << "].size() = " << xj_shares[i].size() << std::endl;
    }

    Rl x_logv_m = detail::interpolate(xjs, ex_seq_lifted, Rl::zero());
    // std::cout << "reconstructed x values" << std::endl;

    /* ===================== 6. verify the result ====================================== */
    std::vector<ChkShare> val(PARTY_NUM);
    for (int i = 0; i < PARTY_NUM; i++)
    {
        auto zero_ = yj_shares[i][0] * x_logv_m - zj_shares[i];
        val[i] = zero_;
    }

    Rl opened_val = detail::interpolate(val, ex_seq_lifted, Rl::zero());

    // std::cout << "opened_val = " << opened_val.force_str() << std::endl;

    assert(opened_val == Rl::zero());

    // std::cout << "zero check passed" << std::endl;
}

#undef PARTY_NUM
#undef COMPRESS_V
#undef RAND_FLAG

#endif