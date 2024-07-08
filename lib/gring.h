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
#include <initializer_list>
#include <concepts>
#include <cmath>
#include <type_traits>

#include <stdexcept>

#include <glog/logging.h>

#include "random.h"

#include "SimpleFIPS202.h"

namespace arith {
 

    /**
     * @note: code by robin
     * @todo: add support for multiplication of 128 bits?
     */
    class int128 {
        public:
            int128() : m_val(_mm_set_epi64x(0, 0)) {}
            int128(__m128i x) : m_val(std::move(x)) {}
            int128(long long x) : m_val(_mm_set_epi64x(0, x)) {}
            int128(long long lo, long long hi) : m_val(_mm_set_epi64x(hi, lo)) {}

            static int128 make_mask(int k) {
                if (k == 0) {
                    return 0;
                } else if (k < 64) {
                    return int128((1ull << k) - 1);
                } else if (k == 64) {
                    return int128(-1);
                } else if (k < 128) {
                    return int128(_mm_set_epi64x((1ull << (k - 64)) - 1, -1));
                } else if (k == 128) {
                    return int128(_mm_set_epi64x(-1, -1));
                } else {
                    __builtin_unreachable();
                }
            }

            int128 operator&(const int128& o) const {
                return m_val & o.m_val;
            }
            int128& operator&=(const int128& o) {
                return *this = *this & o;
            }

            int128 operator^(const int128& o) const {
                return m_val ^ o.m_val;
            }
            int128& operator^=(const int128& o) {
                return *this = *this ^ o;
            }

            int128 operator>>(int s) const {
                __m128i packed_shifted = _mm_srli_epi64(m_val, s); // Shift both parts
                __m128i cross_boundary = m_val;
                if (s < 64) {
                    cross_boundary = _mm_slli_epi64(m_val, 64 - s); // LSB of high to become MSB of low
                } else if (s > 64) {
                    cross_boundary = _mm_srli_epi64(m_val, s - 64); // MSB of high to become LSB of low
                }
                return packed_shifted ^ _mm_srli_si128(cross_boundary, 8); // Right shift the mixin by 8 *bytes* to bring high into low
            }
            int128& operator>>=(int s) {
                return *this = (*this) >> s;
            }

            int128 operator<<(int s) const {
                __m128i packed_shifted = _mm_slli_epi64(m_val, s); // Shift both parts
                __m128i cross_boundary = m_val;
                if (s < 64) {
                    cross_boundary = _mm_srli_epi64(m_val, 64 - s); // MSB of low to become LSB of high
                } else if (s > 64) {
                    cross_boundary = _mm_slli_epi64(m_val, s - 64); // LSB of low to become MSB of high
                }
                return packed_shifted ^ _mm_slli_si128(cross_boundary, 8); // Left shift by 8 bytes to bring low into high
            }
            int128& operator<<=(int s) {
                return *this = (*this) << s;
            }

            bool operator==(const int128& o) const {
                const __m128i tmp = m_val ^ o.m_val;
                return _mm_test_all_zeros(tmp, tmp);
            }
            bool operator!=(const int128& o) const {
                return !(*this == o);
            }

            bool operator<(const unsigned long long& other) { // Special case for this, because it's a pain to do with another int128
                return !(
                        _mm_test_all_zeros(m_val, _mm_set_epi64x(-1, 0))                  // if anything is in the top 64 bits, it's definitely bigger
                     || static_cast<unsigned long long>(_mm_cvtsi128_si64(m_val)) >= other // otherwise, unsigned compare the rest
                     );
            }

            std::int64_t low() const {
                return _mm_extract_epi64(m_val, 0);
            }

            std::int64_t high() const {
                return _mm_extract_epi64(m_val, 1);
            }

            explicit operator __m128i() const {
                return m_val;
            }
            __m128i reveal() const {
                return m_val;
            }

        private:
            __m128i m_val;
    };


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
    
    // TODO: use Keccak instead
    // TODO: figure out a way to use input string array
    //  Prefix_SpongePRG_Fetch(Prefix_SpongePRG_Instance *instance, unsigned char *out, unsigned int outByteLen);
    seed() {
        
    }

    template<T>
    std::vector<bool> to_bits(T a) {
        size_t d = sizeof(T);
        std::vector<bool> res;
        for (int i = 0; i < d; i++) {
            res[i] = a & 1;
            a >>= 1;
        }
        return res;
    }

    template<T>
    T fast_exp(T& a, std::vector<bool> exp) {
        T res = zero();  //TODO: replace zero with default value
        for (int i = size(exp) - 1; i >= 0; i--) {
            if (exp[i]) res *= a;
            a *= a;
        }
        return res;
    }

    // TODO: figure out a seed and length
    template <typename T>
    T random(const unsigned char *seed) {
        unsigned char res[ceil(sizeof(T) / 8)];
        
        if (SHA3_256(res, seed, strlen((const char *)seed))) {
            // Hashing failed.
            counter ++;
        }
        return res;

        // Keccak_HashInstance hi;
        // Keccak_HashInitialize_SHA3_128(&hi);
        // for (int i = 0; i < input_chunks_cnt; i++) {
        //     result = Keccak_HashUpdate(&hi, input[i], strlen((const char *) input[i]) * 8);
        // }
        // int outputByteLen = 32;  // TODO: define an output
        // unsigned char res[outputByteLen]; 
        // //output has a length of one Z2K element, which is decided by size T
        // //in fact at most 128 bits
        // Keccak_HashFinal(&hi, res);
        // return res;
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


/**
 * 
 * Z2k(F, 1)
 * Only supports k = 1 to 64
*/
template<int k>
requires( 1 <= k && k <= 64 )
class Z2k 
{
    public:
        using F = typename arith::datatype<arith::type_idx<k>()>::type;
        
    private:
        static_assert(2 <= k && k <= 32, "Unsupported Base Ring F");

    public:
        static inline F MASK = arith::make_mask<F, k>();

        template <typename T>
        explicit Z2k<k>(const T& ele): val_(F(ele) & MASK) {}

        explicit Z2k<k>(F f, bool /*skip mask*/) : val_(std::move(f & MASK)) {}

        Z2k() : val_(0) {}

    public:
        Z2k<k> operator+(const Z2k& o) const {
            return Z2k<k>(val_ + o.val_, false);
        }

        Z2k<k> operator+=(const Z2k& o) {
            return *this = (*this) + o;
        }

        Z2k<k> operator-(const Z2k& o) const {
            return Z2k<k>(val_ - o.val_, false);
        }

        Z2k<k> operator-=(const Z2k& o) {
            return *this = (*this) + o;
        }

        Z2k<k> operator*(const Z2k& o) const {
            F a = val_;
            F b = o.val_;
            //TODO: to further deal with a bigger than 64 bits
            return Z2k<k>((a * b) & MASK);
        }

        Z2k<k>& operator*=(const Z2k& o) {
            return *this = (*this) * o;
        }

        bool operator==(const Z2k& o) const {
            return (val_ == o.val_);
        }

        bool operator!=(const Z2k& o) const {
            return (val_ != o.val_);
        }

        std::array<bool, k> to_bits() const {
            std::array<bool, k> res;
            F a = val_;
            for (int i = 0; i < k; i++) {
                res[i] = a & 1;
                a >>= 1;
            }
            return res;
        }

        static Z2k<k> from_bits(const std::array<bool, k>& bits) {
           std::int64_t a = 0;
            for (int i = 0; i < std::min(64, k); i++) {
                a |= std::int64_t(bits[i]) << i;
            }
            return Z2k<k>(F(a), true);   
        }

        static Z2k<k> random(PRNG& gen) {
            return Z2k<k>(random<F>(gen));
        }

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
requires ( 1 <= k && k <= 64 && 1 <= d && d <= 32)
class GR1e 
{
    public: 
        using F = typename arith::datatype<arith::type_idx<k>()>::type;
        using BaseType = void;

        template <typename T>
        explicit GR1e<k, d>(const std::array<T, d>& eles): polys_(eles) {}
        explicit GR1e<k, d>(const std::array<Z2k<k>, d>& eles): polys_(eles) {}
        GR1e() { polys_.fill(Z2k<k>());}

        template <typename T>
        explicit GR1e<k, d>(std::initializer_list<T> eles) {
            if (eles.size() != d) {
                throw std::out_of_range("Wrong size of d provided");
            }
            int i = 0;
            for (const auto& ele : eles) {  // initializer list's element only accessed by iterator
                polys_[i] = Z2k<k>(ele);
                i++;
            }
        }
    
    public:
        GR1e<k, d> operator+(const GR1e<k, d>& o) const {
            std::array<Z2k<k>, d> polys;
            for (int i = 0;i < d; i++) {
                polys.emplace_back(o.polys_[i] + polys_[i]);
            }
            return BK<k,d>(polys);
        }

        GR1e<k, d> operator+=(const GR1e<k, d>& o) {
            return *this = (*this) + o;
        }

        GR1e<k, d> operator-(const GR1e<k, d>& o) const {
            std::array<Z2k<k>, d> polys;
            for (int i = 0;i < d; i++) {
                polys.emplace_back(polys_[i] - o.polys_[i]);
            }
            return BK<k,d>(polys);
        }

        GR1e<k, d> operator-=(const GR1e<k, d>& o) {
            return *this = (*this) + o;
        }

        GR1e<k, d> operator*(const GR1e<k, d>& o) const {
            std::array<Z2k<k>, d> a = polys_;
            std::array<Z2k<k>, d> b = o.polys_;
            //TODO: to further deal with a bigger than 64 bits
            return GR1e<k, d>(arith::reduce<k, F>(multiply(a, b)));
        }

        GR1e<k, d>& operator*=(const GR1e<k, d>& o) {
            return *this = (*this) * o;
        }

        bool operator==(const GR1e<k, d>& o) const {
            return (polys_ == o.polys_);
        }

        bool operator!=(const GR1e<k, d>& o) const {
            return (polys_ != o.polys_);
        }
        
        GR1e<k, d> inv() {
            //TODO: enable 128 bits, actually only needs 64 + 32 = 96 bits
            std::vector<bool> exp = arith::to_bits(static_cast<arith::int128>((2^d - 1) * 2^(k - 1) - 1));
            res = arith::fast_exp(*this, exp);
            assert(res * (*this) != one(), "This ring element has no inverse");
            return res;
        }

        static GR1e<k, d> from_bits(const std::array<F, d>& bits) {
            std::array<Z2k<k>, d> a;
            for (int i = 0; i < std::min(64, d); i++) {
                a = Z2k<k>(bits[i]);
            }
            return GR1e<k, d>(a);
        }

        
        std::array<t, d> to_bits() const {
            // TODO
            return res;
        }

        static GR1e<k, d> random(PRNG& gen) {
            std::array<F, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = random<F>(gen); //TODO: the generator elements
            }
            return GR1e<k, d>(res); // auto masked
        }

        static GR1e<k, d> zero() {
            std::array<Z2k<k>, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = Z2k<k>(0); 
            }
            return GR1e<k, d>(res); 
        }

        static GR1e<k, d> one() {
            std::array<Z2k<k>, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = Z2k<k>(1); 
            }
            return GR1e<k, d>(res); 
        }

        template <int m>
        std::array<GR1e<k, d>, m> exceptional_seq() {
            std::array<GR1e<k, d>, m> res;
            static_assert(m <= 2^d, "the ring only has a maximal sequence of at most 2^d length");
            for (int i = 0; i < m; i++) { 
                std::array<F, d> seq;  
                for (int j = 0; j < d; j++) {
                    seq[j] = (i >> j) & 1; //extract jth bit
                }
                res[i] = GR1e<k, d>(seq);
            }
            return res;
        }

        friend ostream &operator<<(ostream &o, const GR1e<k, d>&r)
        { 
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
        std::array<Z2k<k>, d> polys_;
        static constexpr int d0_ = d;

};


/**
 * (GRT1e(GR1e, k))
*/

template<typename T>
struct is_GR1e : std::false_type {};
template<int k, int d>
struct is_GR1e<GR1e<k, d>> : std::true_type {};

//declaration of GRT1e
template<typename R, int k>
struct GRT1e;
template<typename T>
struct is_GRT1e : std::false_type {};
template<typename R, int k>
struct is_GRT1e<GRT1e<R, k>> : std::true_type {};


template<typename T>
concept BaseGR1e = is_GR1e<T>::value && !is_GRT1e<T>::value;

//check if BaseRing ends with GR1e
//TODO: make it more generic ring requirements by checking *, + and polynomial: demo.h example 2
template<typename T>
concept BaseRing = requires {
    requires BaseGR1e<T> || (is_GRT1e<T>::value && requires {
        typename T::BaseType;
        requires BaseGR1e<typename T::BaseType>;
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

        std::array<R, n> low;   
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

/**
 * Towering of Galois Ring
 */
template <BaseRing R, int d>
class GRT1e {
    public:
        // TODO: think up of a better naming for BaseType
        using BaseType = std::conditional_t<is_GR1e<R>::value, R, typename R::BaseType>;

        explicit GRT1e<R, d>(const std::array<R, d>& poly): polys_(poly) {;}

        GRT1e()  { polys_.fill(R());}

        template <typename T>
        explicit GRT1e<R, d>(std::initializer_list<T> eles) {
            if (eles.size() != d) {
                throw std::out_of_range("Unmatched extenssion degree provided");
            }
            std::copy(eles.begin(), eles.end(), polys_.begin());
        }


    public:
        GRT1e<R, d> operator+(const GRT1e<R, d>& o) const {
            std::array<R,d> polys;
            for (int i = 0;i < d0_; i++) {
                polys[i] = polys_[i] + o.polys_[i];
            }
            return GRT1e<R,d>(polys);
        }

        GRT1e<R, d> operator+=(const GRT1e<R, d>& o) {
            return *this = (*this) + o;
        }

        GRT1e<R, d> operator-(const GRT1e<R, d>& o) const {
            std::array<R,d> polys;
            for (int i = 0;i < d0_; i++) {
                polys[i] = polys_[i] - o.polys_[i];
            }
            return GRT1e<R,d>(polys);
        }

        GRT1e<R, d> operator-=(const GRT1e<R, d>& o) {
            return *this = (*this) - o;
        }

        GRT1e<R, d> operator*(const GRT1e<R, d>& o) const {
            return GRT1e<R, d>(reduce(multiply(polys_, o.polys_)));
        }

        GRT1e<R,d>& operator*=(const GRT1e<R,d>& o) {
            return *this = (*this) * other;
        }

        bool operator==(const GRT1e<R, d>& o) const {
            return (polys_ == o.polys_);
        }

        bool operator!=(const GRT1e<R, d>& o) const {
            return (polys_ == o.polys_);
        }

        static GRT1e<R, d> random(PRNG& gen) {
            std::array<R, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = R::random(gen); //TODO: gen
            }
            return GRT1e<R, d>(res); // auto masked
        }

        // TODO: write a constructor for zero()
        static GRT1e<R, d> zero() {
            std::array<R, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = R::zero(); //TODO: gen
            }
            return GRT1e<R, d>(res); 
        }

        GRT1e<R, d> inv() {
            std::vector<bool> exp = arith::to_bits(static_cast<arith::int128>((2^ d_prod_ - 1) * 2^(k - 1) - 1));
            res = arith::fast_exp(*this, exp);
            assert(res * (*this) != one(), "This ring element has no inverse");
            return res;
        }

        template <int m>
        std::array<GRT1e<R, d>, m> exceptional_seq() {
            std::array<GRT1e<R, d>, m> res;
            // TODO: modify m_in int type, TODO: ceil is not static
            static constexpr uint64_t m_in =  static_cast<uint64_t>(
                    std::ceil(static_cast<double>(std::pow(static_cast<double>m, 1.0/d))));
            auto base_seqs = R::exceptional_seq<m_in>();
            static_assert(m <= 2^d_prod_, "the ring only has a maximal sequence of at most 2^{d1 d2 ..dn} length");
            for (int i = 0; i < m; i++) { 
                std::array<R, d> seq;  
                int index = i;
                for (int j = 0; j < d; j++) {
                    seq[j] = base_seqs[index % m_in];
                    index /= m_in;
                }
                res[i] = GRT1e<R, d>(seq);
            }
            return res;
        }

    private:
        // TODO: copy, move constructor for all classes?
        std::array<R, d> polys_;   // from x^0 to x^(d-1), modulus UP TO x^d
        static constexpr int d0_ = d;
        static constexpr int d_prod_ = d * BaseType::d0_;
        static constexpr int tower_depth_ = 1 + (is_GR1e<BaseType>::value ? 0 : BaseType::tower_depth_);
};


#include "brlifttables.h" // br tables for lifting

template <int d, int k0, int k1>
GR1e<d, k1> liftGRT1e(const GR1e<d, k0>& base) {
    static_assert(k1 % k0 == 0, "No subring of correct size exists");
    // TODO:
    return res;
}








#endif
// lifting from the GR(k, k0) - GR(k, k0*k1)

