#ifndef __GALOIS_RING_H
#define __GALOIS_RING_H

#include <emmintrin.h>
#include <smmintrin.h>
#include <wmmintrin.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

#include <glog/logging.h>

#include "random.h"

namespace arith {

    template <int k>
    constexpr int type_idx() {
        static_assert(k > 0, "Sane values?");
        if (k <= 8) return 0;
        else if (k <= 16) return 1;
        else if (k <= 32) return 2;
        else if (k <= 64) return 3;
        else return 4;
    }

    template <int idx> struct datatype;
    template <> struct datatype<0> {using type = uint8_t;};
    template <> struct datatype<1> {using type = uint16_t;};
    template <> struct datatype<2> {using type = uint32_t;};
    template <> struct datatype<3> {using type = uint64_t;};

    template <typename T, int k>
    constexpr T make_mask() {
        return (T(1) << (k % (8 * sizeof(T)))) - 1;
    }
    
    template <typename T>
    T random(PRNG& gen) {
        T res;
        gen.get_random_bytes(reinterpret_cast<uint8_t*>(&res), sizeof(T));
        return res;
    }
    
    template <int k>
    constexpr int num_reduction_monomials() {
        static_assert(2 <= k && k <= 128, "Unsupported extension field");
        int responses[] = {0, 0, 3, 3, 3, 3, 3, 3, 5, 3, 3, 3, 3, 5, 3, 3, 5, 3, 3, 5, 3, 3, 3, 3, 5, 3, 5, 5, 3, 3, 3, 3, 5, 3, 3, 3, 3, 5, 5, 3, 5, 3, 3, 5, 3, 5, 3, 3, 5, 3, 5, 5, 3, 5, 3, 3, 5, 3, 3, 5, 3, 5, 3, 3, 5, 3, 3, 5, 3, 5, 5, 3, 5, 3, 3, 5, 3, 5, 5, 3, 5, 3, 5, 5, 3, 5, 3, 3, 5, 3, 3, 5, 3, 3, 3, 3, 5, 3, 3, 5, 3, 5, 3, 3, 5, 3, 3, 5, 3, 5, 3, 3, 5, 3, 5, 5, 5, 5, 3, 3, 5, 3, 5, 3, 3, 5, 3, 3, 5};
        return responses[k];
    }

    template <int k, int w>
    struct reduction_polynomial_impl {};

    template <int k>
    struct reduction_polynomial_impl<k, 3> {
        using type = int;
        constexpr static type value() {
            type responses[] = {{}, {}, 1, 1, 1, 2, 1, 1, 0, 1, 3, 2, 3, 0, 5, 1, 0, 3, 3, 0, 3, 2, 1, 5, 0, 3, 0, 0, 1, 2, 1, 3, 0, 10, 7, 2, 9, 0, 0, 4, 0, 3, 7, 0, 5, 0, 1, 5, 0, 9, 0, 0, 3, 0, 9, 7, 0, 4, 19, 0, 1, 0, 29, 1, 0, 18, 3, 0, 9, 0, 0, 6, 0, 25, 35, 0, 21, 0, 0, 9, 0, 4, 0, 0, 5, 0, 21, 13, 0, 38, 27, 0, 21, 2, 21, 11, 0, 6, 11, 0, 15, 0, 29, 9, 0, 4, 15, 0, 17, 0, 33, 10, 0, 9, 0, 0, 0, 0, 33, 8, 0, 18, 0, 2, 19, 0, 21, 1, 0};
            return responses[k];
        }
    };

    template <int k>
    struct reduction_polynomial_impl<k, 5> {
        using type = std::tuple<int, int, int>;
        constexpr static type value() {
            type responses[] = {{}, {}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {1, 3, 5}, {0, 0, 0}, {0, 0, 0}, {1, 2, 5}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {1, 3, 4}, {1, 2, 5}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {2, 3, 7}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 4, 6}, {1, 5, 6}, {0, 0, 0}, {3, 4, 5}, {0, 0, 0}, {0, 0, 0}, {3, 4, 6}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {2, 3, 5}, {0, 0, 0}, {2, 3, 4}, {1, 3, 6}, {0, 0, 0}, {1, 2, 6}, {0, 0, 0}, {0, 0, 0}, {2, 4, 7}, {0, 0, 0}, {0, 0, 0}, {2, 4, 7}, {0, 0, 0}, {1, 2, 5}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {1, 2, 5}, {0, 0, 0}, {2, 5, 6}, {1, 3, 5}, {0, 0, 0}, {3, 9, 10}, {0, 0, 0}, {0, 0, 0}, {1, 3, 6}, {0, 0, 0}, {2, 5, 6}, {3, 5, 6}, {0, 0, 0}, {2, 4, 9}, {0, 0, 0}, {1, 3, 8}, {2, 4, 7}, {0, 0, 0}, {1, 2, 8}, {0, 0, 0}, {0, 0, 0}, {2, 6, 7}, {0, 0, 0}, {0, 0, 0}, {1, 5, 8}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {6, 9, 10}, {0, 0, 0}, {0, 0, 0}, {1, 3, 6}, {0, 0, 0}, {1, 6, 7}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {4, 7, 9}, {0, 0, 0}, {2, 4, 5}, {0, 0, 0}, {0, 0, 0}, {3, 4, 5}, {0, 0, 0}, {2, 3, 5}, {5, 7, 8}, {1, 2, 4}, {1, 2, 5}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {1, 2, 6}, {0, 0, 0}, {0, 0, 0}, {5, 6, 7}, {0, 0, 0}, {0, 0, 0}, {1, 2, 7}};
            return responses[k];
        }
    };

    template <int k>
    constexpr auto reduction_polynomial() {
        return reduction_polynomial_impl<k, num_reduction_monomials<k>()>::value();
    }
            
    template <int k, typename T>
    vector<T> reduce_once(const std::vector<T>& x, int red) { // Trinomial
        int n = x.size();
        std::vector<T> hi(n, 0);  // hi poly terms
        hi.insert(hi.begin() + k, x.begin(), x.begin() + n - k);
        std::vector<T> low(x);    // low poly terms
        std::fill_n(low.begin(), n - k, 0);
        std::vector<T> hired(n, 0);     // hi << red
        hired.insert(hired.begin() + red, hi.begin(), hi.begin() + n - red);
        std::vector<T> res;
        for (int i = 0; i < n;i++) {
            res = low[i] - hi[i] - hired[i];
        }
        return res;
    }

    template <int k, typename T>
    T reduce_once(const std::vector<T>& x, const std::tuple<int, int, int>& red) { // Pentanomial
        int n = x.size();
        std::vector<T> hi(n, 0);  // hi poly terms
        hi.insert(hi.begin() + k, x.begin(), x.begin() + n - k);
        std::vector<T> low(x);    // low poly terms
        std::fill_n(low.begin(),n - k, 0);
        std::vector<T> hired1(n, 0);     // hi << red
        hired1.insert(hired1.begin() + std::get<0>(red), hi.begin(), hi.begin() + n - std::get<0>(red));
        std::vector<T> hired2(n, 0);     // hi << red
        hired2.insert(hired2.begin() + std::get<1>(red), hi.begin(), hi.begin() + n - std::get<1>(red));
        std::vector<T> hired3(n, 0);     // hi << red
        hired3.insert(hired3.begin() + std::get<2>(red), hi.begin(), hi.begin() + n - std::get<2>(red));
        std::vector<T> res;
        for (int i = 0; i < n;i++) {
            res = low[i] - hi[i] - hired1[i] - hired2[i] - hired3[i];
        }
        return res;
  }

    /**
     * maximum 32 bits for now
    */
    template <int k, typename T>
    T reduce(const std::uint64_t& x) {
        constexpr auto red = reduction_polynomial<k>();
        if constexpr(k <= 32) {
            std::uint64_t y = reduce_once<k,std::uint64_t>(x,red);
        } 
        // TODO: bigger bits
    }

};


/**
 * 
 * Z2K(F, 1)
 * Only supports k = 2 to 32
*/
template<int k>
class Z2K {
    public:
        using F = typename arith::datatype<arith::type_idx<k>()>::type;
        
    private:
        static_assert(2 <= k && k <= 32, "Unsupported Base Ring F");

    public:
        static inline F MASK = arith::make_mask<F, k>();

        template <typename T>
        explicit Z2K<k>(const T& ele): val_(F(ele) & MASK) {}

        explicit Z2K<k>(F f, bool /*skip mask*/) : val_(std::move(f & MASK)) {}
        
        Z2K<k>() : val_(0) {}

    public:
        Z2K<k> operator+(const Z2K& o) const {
            return Z2K<k>(val_ + o.val_, false);
        }

        Z2K<k> operator+=(const Z2K& o) {
            return *this = (*this) + o;
        }

        Z2K<k> operator-(const Z2K& o) const {
            return Z2K<k>(val_ - o.val_, false);
        }

        Z2K<k> operator-=(const Z2K& o) {
            return *this = (*this) + o;
        }

        Z2K<k> operator*(const Z2K& o) const {
            F a = val_;
            F b = o.val_;
            //TODO: to further deal with a bigger than 64 bits
            return Z2K<k>((a * b) & MASK);
        }

        Z2K<k>& operator*=(const Z2K& o) {
            return *this = (*this) * o;
        }

        bool operator==(const Z2K& o) const {
            return (val_ == o.val_);
        }

        bool operator!=(const Z2K& o) const {
            return (val_ != o.val_);
        }

        // TODO: return value
        std::array<bool, k> to_bits() const {
            std::array<bool, k> res;
            F a = val_;
            for (int i = 0; i < k; i++) {
                res[i] = a & 1;
                a >>= 1;
            }
            return res;
        }

        //TODO: from_bits

        /**
        * To be used only when needing access to the underlying bits, really
        */
        F force_int() const {
            return val_;
        }
    
    public:
        friend std::vector<Z2K<k>> multiply(const std::vector<Z2K<k>>& a, std::vector<Z2K<k>>& b) {
            static_assert(a.size() == b.size(), "Inconsistent multiplication vector size!");
            int n = a.size();
            std::vector<Z2K<k>> mult(n * 2 - 1, 0);
            for (int i = 0; i < n;i++) {
                for (int j = 0; j < n; j++) {
                    mult[i + j] += a[i] * b[j];
                }
            }
            return mult;
        }

    private:
        F val_;

};


template<int k, int d>
class BR {
    private:
        static_assert(1 <= d && d <= 128, "Unsupported Base Ring extension");
        static_assert( polys_.size() == d0_, "Inconsistent Extension with polynomails");
    
    public: 
        using F = typename arith::datatype<arith::type_idx<k>()>::type;

        // 一个约束T只能是整数类型的concept，整数类型包括 char,
        // unsigned char, short, ushort, int, unsinged int, long等。
        template <typename T>
        concept integral = std::is_integral_v<T>;

        template <typename T>
        explicit BR<k,d>(const std::vector<T>& eles): polys_(eles), d0_(d) {}

        explicit BR<k,d>(const std::vector<Z2K<k>>& eles): polys_(eles), d0_(d) {}

        BR<k,d>: d0_(1)  {
            polys_.emplace_back(Z2K<k>());
        }
    
    public:
        BR<k,d> operator+(const BR<k,d>& o) const {
            static_assert(o.d0_ == this.d0_, "Inconsistent Extension Degree");
            std::vector<Z2K<k>> polys;
            for (int i = 0;i < d0_; i++) {
                polys.emplace_back(o.polys_[i] + polys_[i]);
            }
            return BK<k,d>(polys);
        }

        BR<k,d> operator+=(const BR<k,d>& o) {
            return *this = (*this) + o;
        }

        BR<k,d> operator-(const BR<k,d>& o) const {
            static_assert(o.d0_ == this.d0_, "Inconsistent Extension Degree");
            std::vector<Z2K<k>> polys;
            for (int i = 0;i < d0_; i++) {
                polys.emplace_back(polys_[i] - o.polys_[i]);
            }
            return BK<k,d>(polys);
        }

        BR<k,d> operator-=(const BR<k,d>& o) {
            return *this = (*this) + o;
        }

        BR<k,d> operator*(const BR<k,d>& o) const {
            std::vector<Z2K<k>> a = polys_;
            std::vector<Z2K<k>> b = o.polys_;
            //TODO: to further deal with a bigger than 64 bits
            return BR<k, d>(arith::reduce<k, F>(multiply(a, b)));
     
        }

        BR<k,d>& operator*=(const BR<k,d>& o) {
            return *this = (*this) * o;
        }

        bool operator==(const BR<k,d>& o) const {
            return (d0_ == o.d0_ && polys_ == o.polys_);
        }

        bool operator!=(const BR<k,d>& o) const {
            return (d0_ == o.d0_ || polys_ == o.polys_);
        }


    private:
        std::vector<Z2K<k>> polys_(d);
        const int d0_ = 1;
}


/**
 * (GR(BR, k))
*/
template <typename BR, int d>
class GR {
    public:
        GR operator+(const GR& o) const {
            
        }

        GR operator+=(const GR& o) {
            return *this = (*this) + o;
        }

        GR operator-(const GR& o) const {
        }

        GR operator-=(const GR& o) {
            return *this = (*this) + o;
        }

        GR operator*(const GR& o) const {

        }

        GR& operator*=(const GR& o) {
            return *this = (*this) * other;
        }

        bool operator==(const GR& other) const {

        }

        bool operator!=(const GR& other) const {
            
        }
    
    public:
        lift();
        root();
        reduction();


    private:
        /**
         * GR(basering, k )
        */
        BR basering_;
        int k_;
};


#endif
// lifting from the GR(k, k0) - GR(k, k0*k1)

