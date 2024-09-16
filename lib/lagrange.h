#include "gring.h"


namespace detail {

    // TODO: move it to lagrange
    double log_base(double base, double x) {
        return log(x) / log(base);
    }


    template <int d>
    std::array<bool, d> to_bits(std::int64_t value) {
        std::array<bool, d> bits;
        for (std::size_t i = 0; i < d; ++i) {
            bits[i] = (value >> i) & 1;
        }
        return bits;
    }


    template <typename R>
    R lagrange_l(const std::int64_t& lo, const std::int64_t& hi, const R& alpha_i, R x) {
        R res = R::one();
        R denom = R::one();
        for (std::int64_t j = lo; j < hi; j++) {
            R alpha_j = R::from_bits(to_bits<R::d>(j));
            if (alpha_j == alpha_i) continue;
            res *= (x - alpha_j);
            denom *= (alpha_i - alpha_j);
        }
        return res * denom.inv();
    }

    template <typename R>
    R lagrange_l(const std::vector<R>& xcoords, R alpha_i, R x) {
        R res = R::one();
        R denom = R::one();
        for (const auto& alpha_j : xcoords) {
            if (alpha_j == alpha_i) continue;
            res *= (x - alpha_j);
            denom *= (alpha_i - alpha_j);
        }
        return res * denom.inv();
    }

    template <typename R>
    std::vector<R> lagrange_coeff(const std::vector<R>& xcoords, const std::vector<R>& ycoords) {
        std::vector<R> res(xcoords.size());
        for (int i = 0; i < xcoords.size(); i++) {
            std::vector<R> coeffs = point_coeff(i, xcoords);
            for (int k = 0; k < xcoords.size(); k++) {
                res[k] += ycoords[i] * coeffs[k];
            }
        }
        return res;
    }

    /**
     * subfunction of lagrange_coeff
     */
    template <typename R>
    std::vector<R> point_coeff(uint64_t i, const std::vector<R>& xcoords) {
        int n = xcoords.size();
        std::vector<R> res;
        R denom = R::one();
        for (int k = 0; k < n; k++) {
            if (k == i) continue;
            denom *= (xcoords[i] - xcoords[k]);
        }
        res.push_back(denom.inv());  //  1/((x_i-x_0)* ... *(x_i-x_{i-1})*(x_i-x_{i+1})*...*(x_i-x_n))
        
        for (int k = 0; k < n; k++) {
            if (k == i) continue;
            else {   
                res.insert(res.begin(), R::zero());
                if (k < i) {
                    for (int j = 0; j <= k; j++) {
                        res[j] -= res[j + 1] * xcoords[k];
                    }
                }
                else {
                    for (int j = 0; j <= k - 1; j++) {
                        res[j] -= res[j + 1] * xcoords[k];
                    }
                }
            }
        }
        return res;

            // std::vector<R> new_coeffs(k + 1); 
            // int h = k + 2;
            // if (k < i) {
            //     new_coeffs.push_back(R::zero());
            //     h = k + 1;
            //     for (int j = k; j >= 0; j--) {
            //         new_coeffs[j + 1] += res[j];
            //         new_coeffs[j] -= xcoords[k] * res[j];
            //     }
            // } else {
            //     for (int j = k - 1; j >= 0; j--) {
            //         new_coeffs[j + 1] += res[j];
            //         new_coeffs[j] -= xcoords[k] * res[j];
            //     }
            // }
            // for (int j = h; j >= 0; j--) { 
            //     res[j] = new_coeffs[j];
            // }
            // res = new_coeffs;
    }

    template <typename R>
    R interpolate(const std::vector<R>& ys, const R& x) {
        uint64_t d_prod = R::get_d();
        assert(ys.size() < (1ull << static_cast<uint64_t>(std::pow(2, d_prod))));
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
    R dot_product(const std::vector<R>& a, const std::vector<R>& b) {
        R res;
        for (int i = 0; i < a.size(); i++) {
            res += a[i] * b[i];
        }
        return res;
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



}