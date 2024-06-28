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
#include <array>

#include <concepts>
#include <type_traits>

#include <glog/logging.h>

#include "random.h"

#include "KeccakHash.h"

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
    
    // TODO: 想想这个GR存储方式怎么搞。。。
    template <int k>
    constexpr auto reduction_polynomial() {
        return reduction_polynomial_impl<k, num_reduction_monomials<k>()>::value();
    }

};


template <int d, int... ds>
struct reduction_polynomial() {
    static constexpr int count = sizeof...(ds);
    reduction_polynomial<ds...>();
    std::array<d, ds>
};

template <int d>
void reduction_polynomial() {
    std::cout << Last << std::endl;
}

// 递归展开模板参数包，打印每个参数的值
template <int First, int... Rest>
void printValues() {
    std::cout << First << ", ";
    printValues<Rest...>();
}


    template <int k>
    struct reduction_polynomial_impl<k, 5> {
        using type = std::tuple<int, int, int>;
        constexpr static type value() {
            type responses[] = {{}, {}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {1, 3, 5}, {0, 0, 0}, {0, 0, 0}, {1, 2, 5}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {1, 3, 4}, {1, 2, 5}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {2, 3, 7}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 4, 6}, {1, 5, 6}, {0, 0, 0}, {3, 4, 5}, {0, 0, 0}, {0, 0, 0}, {3, 4, 6}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {2, 3, 5}, {0, 0, 0}, {2, 3, 4}, {1, 3, 6}, {0, 0, 0}, {1, 2, 6}, {0, 0, 0}, {0, 0, 0}, {2, 4, 7}, {0, 0, 0}, {0, 0, 0}, {2, 4, 7}, {0, 0, 0}, {1, 2, 5}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {1, 2, 5}, {0, 0, 0}, {2, 5, 6}, {1, 3, 5}, {0, 0, 0}, {3, 9, 10}, {0, 0, 0}, {0, 0, 0}, {1, 3, 6}, {0, 0, 0}, {2, 5, 6}, {3, 5, 6}, {0, 0, 0}, {2, 4, 9}, {0, 0, 0}, {1, 3, 8}, {2, 4, 7}, {0, 0, 0}, {1, 2, 8}, {0, 0, 0}, {0, 0, 0}, {2, 6, 7}, {0, 0, 0}, {0, 0, 0}, {1, 5, 8}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {6, 9, 10}, {0, 0, 0}, {0, 0, 0}, {1, 3, 6}, {0, 0, 0}, {1, 6, 7}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {0, 0, 0}, {4, 7, 9}, {0, 0, 0}, {2, 4, 5}, {0, 0, 0}, {0, 0, 0}, {3, 4, 5}, {0, 0, 0}, {2, 3, 5}, {5, 7, 8}, {1, 2, 4}, {1, 2, 5}, {0, 0, 0}, {0, 0, 0}, {1, 3, 4}, {0, 0, 0}, {1, 2, 6}, {0, 0, 0}, {0, 0, 0}, {5, 6, 7}, {0, 0, 0}, {0, 0, 0}, {1, 2, 7}};
            return responses[k];
        }
    };
    
    // TODO: 想想这个GR存储方式怎么搞。。。
    template <int k>
    constexpr auto reduction_polynomial() {
        return reduction_polynomial_impl<k, num_reduction_monomials<k>()>::value();
    }


/**
 * 
 * Z2K(F, 1)
 * Only supports k = 2 to 32
*/
template<int k>
requires requires {
    1 <= k && k <= 64;
}
class Z2K 
{
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
    private:
        F val_;

};


template<int k, int d>
requires requires {
    1 <= k && k <= 64;
    1 <= d && d <= 32;
}
class BR 
{
    public: 
        using F = typename arith::datatype<arith::type_idx<k>()>::type;
        using BaseType = void;

        template <typename T>
        explicit BR<k, d>(const std::array<T, d>& eles): polys_(eles) {}

        explicit BR<k, d>(const std::array<Z2K<k>, d>& eles): polys_(eles) {}

        BR<k, d>() {
            polys_.fill(Z2K<k>());
        }

        template <typename T>
        explicit BR<k, d>(std::initializer_list<T> eles) {
            if (eles.size() != n) {
                throw std::out_of_range("Wrong size of d provided");
            }
            for (int i = 0;i < d;i++) {
                polys_[i] = Z2K<k>(eles[i]);
            }
        }
    
    public:
        BR<k, d> operator+(const BR<k, d>& o) const {
            std::array<Z2K<k>, d> polys;
            for (int i = 0;i < d; i++) {
                polys.emplace_back(o.polys_[i] + polys_[i]);
            }
            return BK<k,d>(polys);
        }

        BR<k, d> operator+=(const BR<k, d>& o) {
            return *this = (*this) + o;
        }

        BR<k, d> operator-(const BR<k, d>& o) const {
            std::array<Z2K<k>, d> polys;
            for (int i = 0;i < d; i++) {
                polys.emplace_back(polys_[i] - o.polys_[i]);
            }
            return BK<k,d>(polys);
        }

        BR<k, d> operator-=(const BR<k, d>& o) {
            return *this = (*this) + o;
        }

        BR<k, d> operator*(const BR<k, d>& o) const {
            std::array<Z2K<k>, d> a = polys_;
            std::array<Z2K<k>, d> b = o.polys_;
            //TODO: to further deal with a bigger than 64 bits
            return BR<k, d>(arith::reduce<k, F>(multiply(a, b)));
        }

        BR<k, d>& operator*=(const BR<k, d>& o) {
            return *this = (*this) * o;
        }

        bool operator==(const BR<k, d>& o) const {
            return (polys_ == o.polys_);
        }

        bool operator!=(const BR<k, d>& o) const {
            return (polys_ != o.polys_);
        }
        
        // construct from given bits by genmodulus.sage
        static BR<k, d> from_modulus(const F& bits) {

            // std::array<Z2K<k>, d> a;
            // for (int i = 0; i < std::min(64, d); i++) {
            //     a = Z2K<k>(bits[i]);
            // }
            // return BR<k, d>(a);
        }


        static BR<k, d> from_bits(const std::array<F, d>& bits) {
            std::array<Z2K<k>, d> a;
            for (int i = 0; i < std::min(64, d); i++) {
                a = Z2K<k>(bits[i]);
            }
            return BR<k, d>(a);
        }

        friend ostream &operator<<(ostream &o, const BR<k, d>&r)
        { 
            // TODO: 如果最后一项是0，前面多了一个+号。。。我能不能换种方法写对拍器啊。。
            for (int i = d - 1;i > 1;i++) {
                if (r.polys_[i] == 1) { o << "βbar^" << i << " + "; }
                else if (r.polys_[i] == 0) { continue; }
                else { o << r.polys_[i] << "*" << "βbar^" << i << " + "; }
            }
            if (r.polys_[1] == 1) { o << "βbar" << " + "; }
            else if (r.polys_[1] != 0) { o << r.polys_[1] << "*" << "βbar" << " + "; }
            if (r.polys_[0] != 0) o << r.polys_[0];
            return o;            
        }

    private:
        std::array<Z2K<k>, d> polys_;
        static constexpr int d0_ = d;

};


/**
 * (GR(BR, k))
*/

template<typename T>
struct is_BR_template : std::false_type {};
template<int k, int d>
struct is_BR_template<BR<k, d>> : std::true_type {};

//declaration of GR
template<typename R, int k>
struct GR;
template<typename T>
struct is_GR_template : std::false_type {};
template<typename R, int k>
struct is_GR_template<GR<R, k>> : std::true_type {};


template<typename T>
concept BaseBR = is_BR_template<T>::value && !is_GR_template<T>::value;

//check if BaseRing ends with BR
//TODO: make it more generic ring requirements by checking *, + and polynomial: demo.h example 2
template<typename T>
concept BaseRing = requires {
    requires BaseBR<T> || (is_GR_template<T>::value && requires {
        typename T::BaseType;
        requires BaseBR<typename T::BaseType>;
    });
};


namespace reduce {

    template <typename T>
    concept Ring = requires(T t, const T& o) {
         // check *, + 
        { t.operator-=(o) } -> std::same_as<T&>;
        { t.operator-(o) } -> std::same_as<T>;
        { t.operator+=(o) } -> std::same_as<T&>;
        { t.operator+(o) } -> std::same_as<T>;
        { t.operator*=(o) } -> std::same_as<T&>;
        { t.operator*(o) } -> std::same_as<T>;
        { t.operator==(o) } -> std::same_as<std::bool>;
        { t.operator!=(o) } -> std::same_as<std::bool>;

        // // check if there's a ring polynomial stored in an array
        std::convertible_to<decltype(T::polys_ ), std::array<typename T::F, T::d>>;
        std::unsigned_integral<typename decltype(T::polys_)::value_type>;
    };

    // TODO: test this
    // TODO: more constraints?????? idk 
    template <int k, int d, typename R>
    std::array<R, d> reduce_once(const std::array<R, d>& x, int red) { // Trinomial
        std::array<R, d> high;   // high poly terms
        std::copy(x.begin() + k, x.end(), high.begin());
        std::array<R, d> low;   // low poly terms
        std::copy(x.begin(), x.begin() + k, low.begin());

        std::array<R, d> hired;     // hi << red
        std::copy(high.begin() + red, high.begin(), hired.begin());

        std::array<R, d> res;
        for (int i = 0; i < d;i++) {
            res = low[i] - high[i] - hired[i];
        }
        return res;
    }

    template <int k, int d, typename R>
    std::array<R, d> reduce_once(const std::array<R, d>& x, const std::tuple<int, int, int>& red) { // Pentanomial
        std::array<R, d> high;   // high poly terms
        std::copy(x.begin() + k, x.end(), high.begin());
        std::array<R, d> low;   // low poly terms
        std::copy(x.begin(), x.begin() + k, low.begin());

        std::array<R, d> hired1;     // hi << red
        std::copy(high.begin() + std::get<0>(red), high.begin(), hired1.begin());
        std::array<R, d> hired2;     // hi << red
        std::copy(high.begin() + std::get<1>(red), high.begin(), hired2.begin());
        std::array<R, d> hired3;     // hi << red
        std::copy(high.begin() + std::get<2>(red), high.begin(), hired3.begin());

        std::array<R, d> res;
        for (int i = 0; i < d;i++) {
            res = low[i] - high[i] - hired1[i] - hired2[i] - hired3[i];
        }
        return res;
    }

       // TODO: make red polynomial more efficient data structure since it's sparse
    template <int n, int d, typename R> //R: base_ring, n: size of ring polynomial, d: extension degree
    std::array<R, n> reduce_once(const std::array<R, n>& poly_x, const std::array<R, d>& red /* (indx, R_val) */) {
        std::array<R, n> high;  // TODO: new a zero value in R
        std::copy(poly_x.begin() + d, poly_x.end(), high.begin());

        std::array<R, n> low;   // TODO: new a zero value in R
        std::copy(poly_x.begin(), poly_x.begin() + d, low.begin());

        //TODO: red polynomial do not pass on its 最高项， 所以这里是没有最高项的modulus
        for (int i = 0;i < n - d; i++) {
            for (int shift = 0; shift < d;shift++) {
                low[shift + i] -= red[shift] * high[i];
            }
        }
        return low;
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

    template <int n, int d, typename R>
    T reduce(const std::array<R, n>& x) {
        // TODO: constexpr auto red = reduction_polynomial<d>();
        auto& low = reduce_once<n, d, R>(x, red);
        low = reduce_once<n, d, std::uint64_t>(low, red);
        // truncate to d terms
        std::array<R, d> res;
        std::copy(low.begin(), low.begin() + d, res().begin());
        return res;
    }
    
    // TODO: requires写一个。
    template <int n, typename R>
    std::array<R, n * 2 - 1> multiply(const std::array<R, n>& a, std::array<R, n>& b) {
        std::array<R, n * 2 - 1> mult;
        for (int i = 0; i < n;i++) {
            for (int j = 0; j < n; j++) {
                mult[i + j] += a[i] * b[j];
            }
        }
        return mult;
    }
}

template <BaseRing R, int d>
class GR {
    public:
        using BaseType = std::conditional_t<is_BR_template<R>::value, R, typename R::BaseType>;

        explicit GR<R,d>(const std::array<R>& poly): polys_(poly) {;}

        GR<R,d>()  { polys_.fill(R());}

        GR<R,d>(std::array<R, d> polys) : polys_(polys) {}

        template <typename T>
        explicit GR<R, d>(std::initializer_list<T> eles) {
            if (eles.size() != n) {
                throw std::out_of_range("Wrong size of d provided");
            }
            std::copy(ele.begin(), ele.end(), polys_.begin());
        }

    public:
        GR<R,d> operator+(const GR<R,d>& o) const {
            std::array<R,d> polys;
            for (int i = 0;i < d0_; i++) {
                polys[i] = polys_[i] + o.polys_[i];
            }
            return GR<R,d>(polys);
        }

        GR<R,d> operator+=(const GR<R,d>& o) {
            return *this = (*this) + o;
        }

        GR<R,d> operator-(const GR<R,d>& o) const {
            std::array<R,d> polys;
            for (int i = 0;i < d0_; i++) {
                polys[i] = polys_[i] - o.polys_[i];
            }
            return GR<R,d>(polys);
        }

        GR<R,d> operator-=(const GR<R,d>& o) {
            return *this = (*this) - o;
        }

        GR<R,d> operator*(const GR<R,d>& o) const {
            return GR<R, d>(reduce(multiply(polys_, o.polys_)));
        }

        GR<R,d>& operator*=(const GR<R,d>& o) {
            return *this = (*this) * other;
        }

        bool operator==(const GR<R,d>& o) const {
            return (polys_ == o.polys_);
        }

        bool operator!=(const GR<R,d>& o) const {
            return (polys_ == o.polys_);
        }

    private:
        // TODO: copy, move constructor for all classes. ANNOYING
        std::array<R, d> polys_;   // from x^0 to x^(d-1), modulus UP TO x^d
        static constexpr int d0_ = d;
        int layer_;  //TODO: 计算出layer
};



#include "brlifttables.h" // br tables for lifting

template <int d, int k0, int k1>
BR<d, k1> liftGR(const BR<d, k0>& base) {
    static_assert(k1 % k0 == 0, "No subring of correct size exists");
    // TODO:
    return res;
}







#endif
// lifting from the GR(k, k0) - GR(k, k0*k1)

