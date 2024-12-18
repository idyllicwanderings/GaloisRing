#ifndef __GALOIS_RING_H
#define __GALOIS_RING_H

//TODO: 以下四个东西。。compilation wrong
// #include <emmintrin.h>
// #include <smmintrin.h>
// #include <wmmintrin.h>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>


#include <stack>
#include <array>
#include <tuple>
#include <string>
#include <sstream>
#include <initializer_list>
#include <concepts>
#include <cmath>
#include <type_traits>
#include <span>

// DELETE: for DEBUGGING
#include <bitset>
#include <ctime>
#include <random>


#include "random.h"

namespace arith {

    enum MULT_TYPE {
        KA_ONE_ITER,
        KA_RECURSIVE,
        KA_RECURSIVE_DUMMY,
        PLAIN
    };
 

    /**
     * @note: 128 bit code by robin
     * @todo: add support for multiplication of 128 bits?
     */
    
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
        if (k % (8 * sizeof(T)) == 0) return (T(0)) - 1;
        return (T(1) << (k % (8 * sizeof(T)))) - 1;
    }
    
    // TODO: use Keccak instead
    // TODO: figure out a way to use input string array
    //  Prefix_SpongePRG_Fetch(Prefix_SpongePRG_Instance *instance, unsigned char *out, unsigned int outByteLen);

    template<typename T>
    std::vector<bool> to_bits(T& a) {
        size_t d = sizeof(T);
        std::vector<bool> res;
        for (int i = 0; i < d; i++) {
            res[i] = a & 1;
            a >>= 1;
        }
        return res;
    }

    template<typename T>
    T fast_exp(T a, std::vector<bool> exp) {
        T res = T::one();
        for (int i = size(exp) - 1; i >= 0; i--) {
            if (exp[i]) res *= a;
            a *= a;
        }
        return res;
    }

    template<typename T>
    T fast_exp_fermat(T a, int d, int k) {
        T res = T::one();
        for (int i = 0; i < k + d - 1; i++) {
            if (i != k - 1) res *= a;
            a *= a;
        }
        return res;
    }

    // TODO: figure out a seed and length
    template <typename T>
    T random(const unsigned char *seed) {
        
        // unsigned char res[ceil(sizeof(T) / 8)];
        
        // if (SHA3_256(res, seed, strlen((const char *)seed))) 
        // // not using bytes and careful with strlen 
        // {
        //     // Hashing failed.
        //     counter ++;
        // }
        // return res;

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

    std::string trim(const std::string &s) {
        auto start = s.find_first_not_of(" \t\n\r");
        auto end = s.find_last_not_of(" \t\n\r");
        return start == std::string::npos ? "" : s.substr(start, end - start + 1);
    }

    
    /**
     * @brief: 128 bit integer class 
     * @todo: multiplication and addition by SIMD SSE family
     * 
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

            int128 operator*(const int128& o) const {
                __m128i a0 = m_val;                  
                __m128i a1 = _mm_srli_si128(m_val, 8);

                __m128i b0 = o.m_val;                    
                __m128i b1 = _mm_srli_si128(o.m_val, 8); 

                __m128i low = _mm_mul_epu32(a0, b0); 
                __m128i high = _mm_mul_epu32(a1, b1);

                __m128i mid1 = _mm_mul_epu32(a0, b1);
                __m128i mid2 = _mm_mul_epu32(a1, b0);

                __m128i mid = _mm_add_epi64(mid1, mid2);

                __m128i carry = _mm_srli_epi64(mid, 32); 

                low = _mm_add_epi64(low, mid);
                high = _mm_add_epi64(high, carry);

                __m128i res = _mm_unpacklo_epi64(low, high);

                return res;
            }

            int128 operator*=(const int128& o) {
                return *this = *this * o;
            }

            int128 operator+(const int128& o) const {
                //TODO: SIMD addition
            }

            int128& operator+=(const int128& o) {
                return *this = *this + o;
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


    class int256 {

    }

    inline std::ostream& operator<<(std::ostream& os, const int128& x) {
        return os << std::hex << std::setfill('0') << std::setw(16) << x.high()
            << std::hex << std::setfill('0') << std::setw(16) << x.low();
    }

    template <typename T>
    T extract(const int128& x) {
        if constexpr(std::is_same_v<T, int128>) {
            return x;
        } else {
            return T(x.low());
        }
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
        static inline F MASK = arith::make_mask<F, k>();

        template <typename T>
        constexpr explicit Z2k<k>(const T& ele): val_(F(ele) & MASK) {
        }

        explicit Z2k<k>(F f, bool /*skip mask*/) : val_(std::move(f & MASK)) {

        }

        Z2k() : val_(0) {}

    public:
        Z2k<k> operator+(const Z2k& o) const { 
            return Z2k<k>(val_ + o.val_, false); }

        Z2k<k> operator+=(const Z2k& o) { return *this = (*this) + o; }

        Z2k<k> operator-(const Z2k& o) const { return Z2k<k>(val_ - o.val_, false); }

        Z2k<k> operator-=(const Z2k& o) { return *this = (*this) - o; }

        Z2k<k> operator*(const Z2k& o) const {
            F a = val_;
            F b = o.val_;
            //TODO: to further deal with a bigger than 64 bits
            return Z2k<k>((a * b) & MASK);
        }

        Z2k<k>& operator*=(const Z2k& o) { return *this = (*this) * o; }

        bool operator==(const Z2k& o) const { return (val_ == o.val_); }

        bool operator!=(const Z2k& o) const { return (val_ != o.val_); }

        //Z2k<k>& operator=(const Z2k& o) const { return *this = o; }

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
        

        static inline Z2k<k> random_element(randomness::RO& ro) {
            // randomness::RO ro;
            // ro.gen_random_bytes(); 
            size_t byte_len = (k + 7) / 8;
            uint8_t out[byte_len];
            ro.get_bytes(out, byte_len);
            return Z2k<k>(randomness::to_int(out, k));
            //std::srand(time(NULL));
            // return Z2k<k>(std::rand() % (static_cast<int>(std::pow(2, k))));

        }

        static inline Z2k<k> zero() {
            return Z2k<k>(0);
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

// template<int k, int d>
// class GR1e;

template<typename R, int d>
class GRT1e;


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

        explicit GR1e(const std::vector<Z2k<k>>& vec) {
            if (vec.size() != d) { throw std::out_of_range("Vector size does not match the expected array size.");}
            std::copy(vec.begin(), vec.end(), polys_.begin());
        }


        
        template <typename T>
        constexpr explicit GR1e<k, d>(const T& ele) {
            polys_.fill(Z2k<k>());
            polys_[0] = Z2k<k>(ele);
        }

        template <typename T>
        constexpr explicit GR1e<k, d>(std::initializer_list<T> eles) {
            // if (eles.size() != d) {
            //     throw std::out_of_range("Wrong size of d provided");
            // }
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
                polys[i] = o.polys_[i] + polys_[i];
            }
            return GR1e<k,d>(polys);
        }

        GR1e<k, d> operator+=(const GR1e<k, d>& o) { return *this = (*this) + o; }

        GR1e<k, d> operator-(const GR1e<k, d>& o) const {
            std::array<Z2k<k>, d> polys;
            for (int i = 0;i < d; i++) {
                polys[i] = (polys_[i] - o.polys_[i]);
            }
            return GR1e<k,d>(polys);
        }

        GR1e<k, d> operator-=(const GR1e<k, d>& o) { return *this = (*this) - o; }

        GR1e<k, d> operator*(const GR1e<k, d>& o) const;
        // {
        //     std::array<Z2k<k>, d> a = polys_;
        //     std::array<Z2k<k>, d> b = o.polys_;
        //     //TODO: to further deal with a bigger than 64 bits
        //     return GR1e<k, d>(ops::reduce<k, 2*d - 1, d, Z2k<k>>(ops::multiply<d, Z2k<k>>(a, b)));
        // }

        GR1e<k, d>& operator*=(const GR1e<k, d>& o);
        //{ return *this = (*this) * o; }

        bool operator==(const GR1e<k, d>& o) const { return (polys_ == o.polys_); }

        bool operator!=(const GR1e<k, d>& o) const { return (polys_ != o.polys_); }

        Z2k<k>& operator[](int i) {
            if( i < 0 || i >= d ) { throw std::out_of_range("Index out of range"); }
            return polys_[i];
        }

        const Z2k<k>& operator[](int i) const {
            if( i < 0 || i >= d ) { throw std::out_of_range("Index out of range"); }
            return polys_[i];
        }

        static constexpr int get_d() {
            return d0_;
        }

        static constexpr int get_k() {
            return k_;
        }
        
        GR1e<k, d> inv() {
            GR1e<k, d> res = arith::fast_exp_fermat<GR1e<k, d>>(*this, d, k);
            //std::cout << "res: " << res.force_str() << std::endl;
            auto tmp = res * (*this);
            // std::cout << "tmp: " << tmp.force_str() << std::endl;
            //TODO: uncommment this 
            // assert(res * (*this) == one());
            return res;
        }

        /**
         * @note: only used for mapping lagrange pairs (int - GR1e element), 
         *       not for general use to build GR1e element from int
         */
        
        static GR1e<k, d> from_bits(const std::array<bool, d>& value) {
            std::array<Z2k<k>, d> a;
            for (std::size_t i = 0; i < std::min(64, d); i++) {
                Z2k<k>(value[i]);
            }
            return GR1e<k, d>(a);
        }
        

        static GR1e<k, d> from_list(const std::string& str) {
            std::array<Z2k<k>, d> res;
            auto start = str.find_first_not_of(" \[");  //trim the brackets
            auto end = str.find_last_not_of(" \]");
            std::istringstream ss(str.substr(start, end - start + 1));
            std::string cur;
            int i = 0;
            while (std::getline(ss, cur, ',')) {
                res[i++] = Z2k<k>(std::stoi(arith::trim(cur)));
            }
            return GR1e<k, d>(res);
        }
        
        std::array<F, d> force_list() const {
            std::array<F, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = polys_[i].force_int();
            }
            return res;
        }

        std::string force_bits_str() const {
            std::stringstream ss;
            ss << "[";
            for (int i = 0; i < d; i++) {
                int a = int(polys_[i].force_int());
                ss << std::bitset<sizeof(a)*8>(a);
                if (i == d - 1) break;
                ss << ", ";
            }
            ss << "]";
            return ss.str();
        }

        std::string force_str() const {
            std::stringstream ss;
            ss << "[";
            for (int i = 0; i < d; i++) {
                ss << int(polys_[i].force_int());
                if (i == d - 1) break;
                ss << ", ";
            }
            ss << "]";
            return ss.str();
        }

        static inline GR1e<k, d> random_element(randomness::RO& ro) {
            std::array<Z2k<k>, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = Z2k<k>::random_element(ro); 
            }
            return GR1e<k, d>(res); 
        }

        static inline std::vector<GR1e<k, d>> random_vector(int m, randomness::RO& ro) {
            std::vector<GR1e<k, d>> res;
            for (int i = 0; i < m; i++) {
                res.push_back(random_element(ro));
            }
            return res;
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
            res[0] = Z2k<k>(1); 
            return GR1e<k, d>(res); 
        }

        static std::vector<GR1e<k, d>> exceptional_seq(int m) {
            std::vector<GR1e<k, d>> res(m);
            assert(m <= (1 << d));
            // , "the ring only has a maximal sequence of at most 2^d length");
            for (int i = 0; i < m; i++) { 
                std::vector<Z2k<k>> seq(d);  
                for (int j = 0; j < d; j++) {
                    seq[j] = Z2k<k>((i >> j) & 1); //extract jth bit
                }
                res[i] = GR1e<k, d>(seq);
            }
            return res;
        }

        friend std::ostream &operator<<(std::ostream &o, const GR1e<k, d>&r)
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

    static constexpr int d0_ = d;
    static constexpr int k_ = k;

    private:
        std::array<Z2k<k>, d> polys_;

    // temporary use
    public:
        inline static arith::MULT_TYPE MULT_METHOD_ = arith::PLAIN;

};

/**
 * (GRT1e(GR1e, k))
*/
template<typename T>
struct is_gr_template : std::false_type {};
template<int k, int d>
struct is_gr_template<GR1e<k, d>> : std::true_type {};

// template<typename R, int d>
// class GRT1e;
template<typename T>
struct is_grt_template : std::false_type {};
template<typename R, int d>
struct is_grt_template<GRT1e<R, d>> : std::true_type {};


template<typename T>
concept IsBaseRing = is_gr_template<T>::value && !is_grt_template<T>::value;

template<typename T>
concept BaseRing = IsBaseRing<T> || (is_grt_template<T>::value && requires {
    typename T::BaseType;
    requires IsBaseRing<typename T::BaseType>;
});


/**
 * Towering of Galois Ring
 */
template <BaseRing R, int d>
class GRT1e<R, d> {
    public:
        // TODO: think up of a better naming for BaseType
        using BaseType = std::conditional_t<is_gr_template<R>::value, R, typename R::BaseType>;

        explicit GRT1e<R, d>(const std::array<R, d>& poly): polys_(poly) {;}

        GRT1e()  { polys_.fill(R());}

        template <typename T>
        constexpr explicit GRT1e<R, d>(std::initializer_list<T> eles) {
            // if (eles.size() != d) {
            //     throw std::out_of_range("Unmatched extenssion degree provided");
            // }
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

        GRT1e<R, d> operator*(const GRT1e<R, d>& o) const;
        // {
        //     return GRT1e<R, d>(ops::reduce<k_, 2*d - 1, d, R>(ops::multiply<d, R>(polys_, o.polys_)));
        // }

        GRT1e<R, d>& operator*=(const GRT1e<R,d>& o);
        // {
        //     return *this = (*this) * o;
        // }

        bool operator==(const GRT1e<R, d>& o) const {
            return (polys_ == o.polys_);
        }

        bool operator!=(const GRT1e<R, d>& o) const {
            return (polys_ == o.polys_);
        }

        static constexpr int get_d() {
            return d0_;
        }

        static constexpr int get_k() {
            return k_;
        }

        static inline GRT1e<R, d> random_element(randomness::RO& ro) {
            std::array<R, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = R::random_element(ro); 
            }
            return GRT1e<R, d>(res); 
        }

        static inline std::vector<GRT1e<R, d>> random_vector(int m, randomness::RO& ro) {
            std::vector<GRT1e<R, d>> res(m);
            for (int i = 0; i < m; i++) {
                res.push_back(random_element(ro));
            }
            return res;
        }


        // TODO: write a constructor for zero()
        static GRT1e<R, d> zero() {
            std::array<R, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = R::zero(); //TODO: gen
            }
            return GRT1e<R, d>(res); 
        }

        static GRT1e<R, d> one() {
            std::array<R, d> res;
            res[0] = R::one(); 
            return GRT1e<R, d>(res); 
        }

        GRT1e<R, d> inv() {
            GRT1e<R, d> res = arith::fast_exp_fermat<GRT1e<R, d>>(*this, d_prod_, k_);
            assert(res * (*this) == one());
            return res;
        }


        static std::vector<GRT1e<R, d>> exceptional_seq(int m) {
            std::vector<GRT1e<R, d>> res(m);
            // TODO: modify m_in int type, TODO: ceil is not static
            uint64_t m_in =  std::ceil((std::pow((double)m, 1.0/d)));
            // static constexpr uint64_t m_in =  static_cast<uint64_t>(
            //         std::ceil(static_cast<double>(std::pow(static_cast<double>(m), 1.0/d))));
            auto base_seqs = R::template exceptional_seq<>(m_in);
            assert(m <= 2^d_prod_);
            // , "the ring only has a maximal sequence of at most 2^{d1 d2 ..dn} length");
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

        /**
         * @note: only used for mapping lagrange pairs (int - GRT1e element), not for general use
         */
        static GRT1e<R, d> from_bits(std::array<bool, d> bits) {
            std::array<R, d> res;
            for (std::size_t i = 0; i < d; i++) {
                std::array<bool, d_prod_ / d> in_bits;
                for (std::size_t j = 0; j < d_prod_ / d; j++) {
                    in_bits[j] = bits[i * (d_prod_ / d) + j];
                }
                res[i] = R::from_bits(in_bits);
            }
            return GRT1e<R, d>(res);
        }


        static GRT1e<R, d> from_list(const std::string& str) {
            std::array<R, d> res;
            std::stack<char> sstack;
            std::string cur;
            int i = 0;
            for (char ch: str) {
                if (ch == '[') {
                    if (!sstack.empty()) cur += ch; 
                    sstack.push(ch);
                } 
                else if (ch == ']') {
                    sstack.pop();
                    if (!sstack.empty()) cur += ch;
                    else {
                        res[i++] = R::from_list(arith::trim(cur));
                        cur.clear();
                    }
                } 
                else if (ch == ',' && sstack.size() == 1) {
                    if (!cur.empty()) {
                        res[i++] = R::from_list(arith::trim(cur));
                        cur.clear();
                    }
                } 
                else {
                    if (!sstack.empty()) cur += ch;
                }
            }
            return GRT1e<R, d>(res);
        }

        std::string force_str() const {
            std::stringstream ss;
            ss << "[";
            for (int i = 0; i < d; i++) {
                ss << polys_[i].force_str();
                if (i == d - 1) break;
                ss << ", ";
            }
            ss << "]";
            return ss.str();
        }

        std::string force_bits_str() const {
            std::stringstream ss;
            ss << "[";
            for (int i = 0; i < d; i++) {
                ss << polys_[i].force_bits_str();
                if (i == d - 1) break;
                ss << ", ";
            }
            ss << "]";
            return ss.str();
        }

        static constexpr int k_ = R::k_;
        static constexpr int d0_ = d;
        static constexpr int d_prod_ = d * R::d0_;

    private:
        std::array<R, d> polys_;   // from x^0 to x^(d-1), modulus UP TO x^d
        static constexpr int tower_depth_ = 1 + (is_gr_template<BaseType>::value ? 0 : BaseType::tower_depth_);
};



namespace ops {
    
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

    template <int k>
    struct integer_factor_impl {
        using type = int;
        constexpr inline static type value() {
            type responses[] = {
                0,             // 0 
                1,            // 1 
                2,            // 2
                3,            // 3
                2,            // 4
                5,            // 5
                3,            // 6
                7,            // 7
                2,            // 8
                3,            // 9
                5,            // 10
                11,            // 11
                3,            // 12
                13,            // 13
                7,            // 14
                5,            // 15
                2,            // 16
                17,            // 17
                3,            // 18
                19,            // 19
                5,            // 20
                7,            // 21
                11,            // 22
                23,            // 23
                3,            // 24
                5,            // 25
                13,            // 26
                3,            // 27
                7,            // 28
                29,            // 29
                5,            // 30
                31,            // 31
                2,            // 32
                11,            // 33
                17,            // 34
                7,            // 35
                3,            // 36
                37,            // 37
                19,            // 38
                13,            // 39
                5,            // 40
                41,            // 41
                7,            // 42
                43,            // 43
                11,            // 44
                5,            // 45
                23,            // 46
                47,            // 47
                3,            // 48
                7,            // 49
                5,            // 50
                17,            // 51
                13,            // 52
                53,            // 53
                3,            // 54
                11,            // 55
                7,            // 56
                19,            // 57
                29,            // 58
                59,            // 59
                5,            // 60
                61,            // 61
                31,            // 62
                7,            // 63
                2            // 64
        };
            return responses[k];
        }
    };

    template <int n, int d, typename R>
    std::array<R, n> reduce_once(const std::array<R, n>& x, const int& red) { // Trinomial

        std::array<R, n> high;   // high poly terms
        std::copy(x.begin() + d, x.end(), high.begin());

        std::array<R, n> low;   // low poly terms
        std::copy(x.begin(), x.begin() + d, low.begin());

        std::array<R, n> hired;     // hi << red
        std::copy(high.begin(), high.begin() + n - red, hired.begin() + red);

        std::array<R, n> res;
        for (int i = 0; i < d;i++) {
            res[i] = low[i] - high[i] - hired[i];
        }

        // std::cout << "res:";
        // for (int i = 0; i < n; i++) {
        //     int a = int(res[i].force_int());
        //     std::cout << a << ", ";
        // }
        // std::cout << std::endl;
        // std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

        return res;
    }

    template <int n, int d, typename R>
    std::array<R, n> reduce_once(const std::array<R, n>& x, const std::tuple<int, int, int>& red) { // Pentanomial
    

        std::array<R, n> high;   // high poly terms
        std::copy(x.begin() + d, x.end(), high.begin());


        std::array<R, n> low;   // low poly terms
        std::copy(x.begin(), x.begin() + d, low.begin());


        std::array<R, n> hired1;     // hi << red
        std::copy(high.begin(), high.begin() + n - std::get<0>(red), hired1.begin() + std::get<0>(red));
        std::array<R, n> hired2;     // hi << red
        std::copy(high.begin(), high.begin() + n - std::get<1>(red), hired2.begin() + std::get<1>(red));
        std::array<R, n> hired3;     // hi << red
        std::copy(high.begin(), high.begin() + n - std::get<2>(red), hired3.begin() + std::get<2>(red));

        std::array<R, n> res;
        for (int i = 0; i < n;i++) {
            res[i] = low[i] - high[i] - hired1[i] - hired2[i] - hired3[i];
        }
        return res;
    }

       // TODO: make red polynomial more efficient data structure since it's sparse
    template <int n, int d, typename R> //R: base_ring, n: size of ring polynomial, d: extension degree
    std::array<R, n> reduce_once(const std::array<R, n>& poly_x, const std::array<R, d + 1>& red /* (indx, R_val) */) {
        
        std::array<R, n> high;  // high poly terms
        std::copy(poly_x.begin() + d, poly_x.end(), high.begin());

        std::array<R, n> low;   // low poly terms
        std::copy(poly_x.begin(), poly_x.begin() + d, low.begin());

        //TODO: red polynomial do not pass on its 最高项， 所以这里是没有最高项的modulus
        for (int i = 0;i < n - d; i++) {
            for (int shift = 0; shift < d; shift++) {
                low[shift + i] -= red[shift] * high[i];
            }
        }

        return low;
    }
    
    #include "grmodtables.h"
    template <typename R, int k, int d>
    auto get_reduction_polynomial() {
        if constexpr (std::is_same_v<R, Z2k<k>>) {
            return reduction_polynomial<d>();
        } 
        else {
            return grmodtables::reduction_polynomial<k, R, d>;
        }
    }

    //template <int k, int n, typename R, int dlen, std::array<int, dlen> ds>
    template <int k, int n, int d, typename R>
    std::array<R, d> reduce(const std::array<R, n>& x) {
        const auto red = get_reduction_polynomial<R, k, d>();
        // after one reduction, the polynomial will be using the same reduction polynomial
        //.就不对了
        if constexpr (std::is_same_v<R, Z2k<k>>) {
            auto low = reduce_once<n, d, R>(x, red); 
            low = reduce_once<n, d, R>(low, red); 
            std::array<R, d> res;
            std::copy(low.begin(), low.begin() + d, res.begin());
            return res;
        }
        else {
            //std::cout << "I AM SUPPOSED NOT TO BE HERE" << std::endl;
            auto low = reduce_once<n, d, R>(x, red);
            low = reduce_once<n, d, R>(low, red);
            std::array<R, d> res;
            std::copy(low.begin(), low.begin() + d, res.begin());
            return res;
        }
       
    }
    
    template <int n, typename R>
    std::array<R, n * 2 - 1> multiply(const std::array<R, n>& a, const std::array<R, n>& b) {
        std::array<R, n * 2 - 1> mult;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                mult[i + j] += a[i] * b[j];
            }
        }
        return mult;
    }

    template <int n, typename R>
    //std::array<R, n> elewise_add(const std::array<R, n>& a, const std::array<R, n>& b) {
    std::array<R, n> elewise_add(std::span<const R> a, std::span<const R> b) {
        std::array<R, n> res;
        for (int i = 0; i < n; i++) {
            res[i] = a[i] + b[i];
        }
        return res;
    }

    template <int n, typename R>
    //std::array<R, n> elewise_sub(const std::array<R, n>& a, const std::array<R, n>& b) {
    std::array<R, n> elewise_sub(std::span<const R> a, std::span<const R> b) {
        std::array<R, n> res;
        for (int i = 0; i < n; i++) {
            res[i] = a[i] - b[i];
        }
        return res;
    }


    template <int n, typename R>
    //std::array<R, n * 2 - 1> KA_one_iter(const std::array<R, n>& a, const std::array<R, n>& b) {
    std::array<R, n * 2 - 1> KA_one_iter(std::span<const R> a, std::span<const R> b) {
        std::array<R, n * 2 - 1> res; // [0, n - 1]
        std::array<R, n> d;
        for (int i = 0; i < n; i++) {
            d[i] = a[i] * b[i];
        }
        for (int s = 0; s < n - 1; s++) {
            for (int t = s + 1; t < n; t++) {  // extra 2n - 3 addtions
                res[s + t] = res[s + t] + (a[s] + a[t]) * (b[s] + b[t]) - d[s] - d[t];
            }
        }

        res[0] = d[0];
        res[2 * n - 2] = d[n - 1];
        for (int i = 2; i < 2 * n - 2; i += 2) {
            res[i] += d[i / 2];
        }
        return res;
    }

    template <int n, typename R>
    std::array<R, n * 2 - 1> KA_binary(std::span<const R> a, std::span<const R> b) {
        std::array<R, n * 2 - 1> res;
        // if not even number, make it even at first
        constexpr int even_ceil = (n % 2 == 0)? n : n + 1;
        if (even_ceil != n) {
            std::vector<R> a_padded(even_ceil, R::zero());
            std::vector<R> b_padded(even_ceil, R::zero());

            std::copy(a.begin(), a.end(), a_padded.begin());
            std::copy(b.begin(), b.end(), b_padded.begin());

            auto padded = KA_binary<even_ceil, R>(a_padded, b_padded);
            std::array<R, n * 2 - 1> truncated;
            std::copy(padded.begin(), padded.begin() + (n * 2 - 1), truncated.begin());
            return truncated;
        }
        
        std::array<R, n * 2 - 1> res; // [0, n - 1]
        std::array<R, n> d;
        for (int i = 0; i < n; i++) {
            d[i] = a[i] * b[i];
        }
        for (int s = 0; s < n - 1; s++) {
            for (int t = s + 1; t < n; t++) {  // extra 2n - 3 addtions
                res[s + t] = res[s + t] + (a[s] + a[t]) * (b[s] + b[t]) - d[s] - d[t];
            }
        }

        res[0] = d[0];
        res[2 * n - 2] = d[n - 1];
        for (int i = 2; i < 2 * n - 2; i += 2) {
            res[i] += d[i / 2];
        }
        return res;
    }

    /**
     * TODO: add dummy coefficients
     *      to see further explanation, the paper
     *      'Generalizations of the Karatsuba Algorithm for Efficient Implementations'
     *      the notations here follows the paper.
     *      
     *      Tackling the overlapping parts:
     *      We have n coefficients -> m "coefficients" with each coefficient being 
     *          a polynomial of nn coefficients:
     *              e.g. a_n*x^n + a_{n-1}*x^{n-1} + ... + a_0
     *                  -> A_{m-1}*x^{(m-1)*nn} + A_{m-2}*x^{(m-2)*nn}... + A_0.
     *      so when n = 6, m = 2, nn = 3: 
     *          a_5*x^5 + a_4*x^4 + a_3*x^3 + a_2*x^2 + a_1*x + a_0 -> A_1*x^3 + A_0,
     *          where deg(A_1) = deg(A_0) = nn - 1.
     *      if we consider the multiplication of two polynomials A and B,
     *      where A = A_{m-1}*x^{(m-1)*nn} + A_{m-2}*x^{(m-2)*nn}... + A_0
     *      and B = B_{m-1}*x^{(m-1)*nn} + B_{m-2}*x^{(m-2)*nn}... + B_0.
     *      we can obtain C = A * B = C_{2m-2}*x^{(2m-2)*nn} + C_{2m-3}*x^{(2m-3)*nn} + ... + C_0,
     *      where deg(C_i) = 2*nn - 2,
     *      so the overlapping part has (i*nn + 2*nn - 2) - (i+1)*nn + 1 = nn - 1 items.
     * 
     */
    template <int n, typename R> 
    //std::array<R, n * 2 - 1> KA_recursive(const std::array<R, n>& a, const std::array<R, n>& b) {
    std::array<R, n * 2 - 1> KA_recursive(std::span<const R> a, std::span<const R> b, 
                                          bool is_dummy = false) {


        // if n is not a power of 2, pad both arrays to the nearest power of 2
        constexpr int pow2_ceil = 1 << (static_cast<int>(std::ceil(std::log2(n))));

        if (pow2_ceil != n && is_dummy) {
            std::vector<R> a_padded(pow2_ceil, R::zero());
            std::vector<R> b_padded(pow2_ceil, R::zero());

            std::copy(a.begin(), a.end(), a_padded.begin());
            std::copy(b.begin(), b.end(), b_padded.begin());

            auto padded = KA_recursive<pow2_ceil, R>(a_padded, b_padded);
            std::array<R, n * 2 - 1> truncated;
            std::copy(padded.begin(), padded.begin() + (n * 2 - 1), truncated.begin());
            return truncated;
        }
    
        constexpr int m = integer_factor_impl<n>::value();
        if (m == n) {
            return KA_one_iter<n, R>(a, b);
        } 
        constexpr size_t nn = static_cast<std::size_t>(n / m);

        std::array<std::span<const R>, m> aa, bb;
        //std::vector<std::span<R>> aa(m), bb(m);
        for (int i = 0; i < m; i++) {
            aa[i] = std::span<const R>(a.data() + i * nn, nn);
            bb[i] = std::span<const R>(b.data() + i * nn, nn);
        }
        std::array<std::array<R, 2 * nn - 1>, m> d;
        for (int i = 0; i < m; ++i) {
            d[i] = KA_recursive<nn, R>(aa[i], bb[i]);
        }
        std::array<std::array<R, 2 * nn - 1>, 2 * m - 1> dst;
        for (int i = 0; i < 2 * m - 1; ++i) {
            dst[i].fill(R::zero());
        }
        for (int s = 0; s < m - 1; s++) {
            for (int t = s + 1; t < m; t++) {
                std::array<R, nn> aast = elewise_add<nn, R>(aa[s], aa[t]);
                std::array<R, nn> bbst = elewise_add<nn, R>(bb[s], bb[t]);
                std::array<R, 2 * nn - 1> tmp = KA_recursive<nn, R>(aast, bbst);
                dst[s + t] = elewise_add<2 * nn - 1, R>(dst[s + t], tmp);
                tmp = elewise_add<2 * nn - 1, R>(d[s], d[t]);
                dst[s + t] = elewise_sub<2 * nn - 1, R>(dst[s + t], tmp);
                if (!((s + t) % 2)) {
                    dst[s + t] = elewise_add<2 * nn - 1, R>(dst[s + t], d[(s + t) / 2]);
                }
            }
        }

        std::array<R, 2 * n - 1> res;
        auto v = res.begin();
        std::copy(d[0].begin(), d[0].end(), res.begin());  // d[0].size() - (nn - 1) = 2*nn - 1 - nn + 1 = nn
        v += nn;
        for (int i = 1; i < 2 * m - 2; ++i) {
            for (int j = 0; j < nn - 1; ++j) {
                *v += dst[i][j];
                v++;
            }
            std::copy(dst[i].begin() + nn - 1, dst[i].end(), v);  // 2*nn - 1 - (nn - 1) - (nn - 1) = 1
            v++;
        }
        for (int j = 0; j < nn - 1; ++j) {
            *v += d[m - 1][j];
            v++;
        }
        std::copy(d[m - 1].begin() + nn - 1, d[m - 1].end(), v);
        return res;
    }



    /*
    template <int n, typename R>
    std::array<R, n * 2 - 1> KA_recursive(const std::array<R, n>& a, const std::array<R, n>& b) {
        // determine the recuisive depth with factorization of n
        std::vector<uint64_t> factors = integer_factor_impl<n>::value();
        int depth = factors.size();
        std::array<R, n * 2 - 1> c;
        if (depth == 1) {
            return KA_one_iter<n, R>(a, b);
        } 
        else {
            // parse the polynomial as a degree of factors[i],
            // with each polynomial of terms of factors[i] + 1, 
            // then with n/factors[i] polynomials, then combine the Di coefficients 
            // by the 1-iter Karatsuba method, notice the treatment of 0 terms in between
            int deg = factors[i];
            int sub_n = n / deg; 

            for (int i = 0; i < deg; ++i) {
                for (int j = 0; j < sub_n; ++j) {
                    A[i][j] = a[i * sub_n + j];
                    B[i][j] = b[i * sub_n + j];
                }
            }
            std::array<R, sub_n> D;
            std::array<R, sub_n> Dij;
            std::array<R, n * 2 - 1> c; // [0, n - 1]
            for (int i = 1; i < sub_n; i++) {
                D[i] = A[i] * B[i];
                for (int s = 0; s < (i + 1) / 2; s++) {
                    int t = i - s;
                    Dij[s] = (a[s] + a[t]) * (b[s] + b[t]); //Dij的长度有问题
                }
            }
            c[0] = D[0];
            c[n * 2 - 2] = D[n - 1];
            for (int i = 1; i < 2 * n - 1; i++) {
                R c_i = R::zero();
                for (int s = 0; s < (i + 1) / 2; s++) {
                    int t = i - s;
                    c_i += Dij[s] - D[s] = D[t];
                }
                if (i % 2 == 0) {
                    c_i += a[i / 2] * b[i / 2];
                }
                c[i] = c_i;
            }
            
            for (int i = 0; i < factor; ++i) {
                c_parts[i] = KA_recursive<sub_n, R>(a_parts[i], b_parts[i]);
            }

            // 合并递归结果：将子多项式组合成最终结果
            // 使用Karatsuba的方法来合并结果
            // 注意：你可能需要更复杂的合并方式来处理多个子多项式

            // TODO: Implement combination logic

            return c;  // 返回组合后的结果
        }
    }

}

    template <int n, int m, typename R>  // 这里的a,b都是初始的多项式
    std::array<R, 2 * m - 1> KA_recursive_inner(const std::array<R, n>& a, 
                                                const std::array<R, n>& b
                                                uint64_t i) {
        // parse the polynomial as a degree of factors[i],
        // with each polynomial of terms of factors[i] + 1, 
        // then with n/factors[i] polynomials, then combine the Di coefficients 
        // by the 1-iter Karatsuba method, notice the treatment of 0 terms in between
        std::vector<uint64_t> factors = integer_factor_impl<n>::value();
        int depth = factors.size();
        uint64_t fac = factors[i];

        // fac degree, but with n/fac polynomials

        std::array<R, sub_n> D;
        std::array<R, sub_n> Dij;
        std::array<R, n * 2 - 1> c; // [0, n - 1]
            for (int i = 1; i < sub_n; i++) {
                D[i] = A[i] * B[i];
                for (int s = 0; s < (i + 1) / 2; s++) {
                    int t = i - s;
                    Dij[s] = (a[s] + a[t]) * (b[s] + b[t]); //Dij的长度有问题
                }
            }
            c[0] = D[0];
            c[n * 2 - 2] = D[n - 1];
            for (int i = 1; i < 2 * n - 1; i++) {
                R c_i = R::zero();
                for (int s = 0; s < (i + 1) / 2; s++) {
                    int t = i - s;
                    c_i += Dij[s] - D[s] = D[t];
                }
                if (i % 2 == 0) {
                    c_i += a[i / 2] * b[i / 2];
                }
                c[i] = c_i;
            }
            
            for (int i = 0; i < factor; ++i) {
                c_parts[i] = KA_recursive<sub_n, R>(a_parts[i], b_parts[i]);
            }

            // 合并递归结果：将子多项式组合成最终结果
            // 使用Karatsuba的方法来合并结果
            // 注意：你可能需要更复杂的合并方式来处理多个子多项式

            // TODO: Implement combination logic

            return c;  // 返回组合后的结果
        }
    }
    */

}


template <int k, int d>
GR1e<k, d> GR1e<k, d>::operator*(const GR1e<k, d>& o) const {
    std::array<Z2k<k>, d> a = polys_;
    std::array<Z2k<k>, d> b = o.polys_;
    //TODO: to further deal with a bigger than 64 bits
    if (GR1e<k, d>::MULT_METHOD_ == arith::KA_ONE_ITER) {
        return GR1e<k, d>(ops::reduce<k, 2*d - 1, d, Z2k<k>>(ops::KA_one_iter<d, Z2k<k>>(a, b)));
    }
    else if (GR1e<k, d>::MULT_METHOD_ == arith::KA_RECURSIVE) {
        return GR1e<k, d>(ops::reduce<k, 2*d - 1, d, Z2k<k>>(ops::KA_recursive<d, Z2k<k>>(a, b)));
    }
    else if (GR1e<k, d>::MULT_METHOD_ == arith::KA_RECURSIVE_DUMMY) {
        return GR1e<k, d>(ops::reduce<k, 2*d - 1, d, Z2k<k>>(ops::KA_recursive<d, Z2k<k>>(a, b, true)));
    }
    return GR1e<k, d>(ops::reduce<k, 2*d - 1, d, Z2k<k>>(ops::multiply<d, Z2k<k>>(a, b)));
}


template <int k, int d>
GR1e<k, d>& GR1e<k, d>::operator*=(const GR1e<k, d>& o) { return *this = (*this) * o; }

template <BaseRing R, int d>
GRT1e<R, d> GRT1e<R, d>::operator*(const GRT1e<R, d>& o) const {
    return GRT1e<R, d>(ops::reduce<k_, 2*d - 1, d, R>(ops::multiply<d, R>(polys_, o.polys_)));
}


template <BaseRing R, int d>
GRT1e<R, d>& GRT1e<R, d>::operator*=(const GRT1e<R,d>& o) {
    return *this = (*this) * o;
}


#include "grlifttables.h"

// lifting from the GR(k, k0) - GR(k, k0*k1)
template <int k, int d0, int d1>
GR1e<k, d1> liftGR(const GR1e<k, d0>& base) {
    static_assert(d1 % d0 == 0, "No subring of correct size exists");
    GR1e<k, d1> res;
    for (int j = 0; j < d1; j++) {
        Z2k<k> x_i;
        for (int i = 0; i < d0; i++) {
            x_i += base[i] * grlifttables::lift_v<k, d0, d1>[i][j];
        }
        res[j] = x_i;
    }
    return res;
}

/**
 * @brief: p-adic extension from Z2^k0 to Z2^k1
 */
template <int k0, int k1, int d>
GR1e<k1, d> extendGR(const GR1e<k0, d>& base) {
    static_assert(k1 > k0, "Z2k must be larger");
    GR1e<k1, d> res;
    for (int i = 0; i < d; i++) {
        res[i] = Z2k<k1>(base[i].force_int());
    }
    return res;
}



#endif

