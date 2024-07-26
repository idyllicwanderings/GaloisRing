#ifndef __GALOIS_RING_H
#define __GALOIS_RING_H

//TODO: 以下四个东西。。compilation wrong
// #include <emmintrin.h>
// #include <smmintrin.h>
// #include <wmmintrin.h>
// #include <iomanip>
// #include <algorithm>
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


// #include "SimpleFIPS202.h"

namespace arith {
 

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
    T fast_exp(T& a, std::vector<bool> exp) {
        T res;  
        for (int i = size(exp) - 1; i >= 0; i--) {
            if (exp[i]) res *= a;
            a *= a;
        }
        return res;
    }

    template<typename T>
    T fast_exp_fermat(T&a, int d, int k) {
        T res; 
        for (int i = 0; i < k + d - 2; i++) {
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

        Z2k<k> operator-=(const Z2k& o) { return *this = (*this) + o; }

        Z2k<k> operator*(const Z2k& o) const {
            F a = val_;
            F b = o.val_;
            //TODO: to further deal with a bigger than 64 bits
            return Z2k<k>((a * b) & MASK);
        }

        Z2k<k>& operator*=(const Z2k& o) { return *this = (*this) * o; }

        bool operator==(const Z2k& o) const { return (val_ == o.val_); }

        bool operator!=(const Z2k& o) const { return (val_ != o.val_); }

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
        
        static Z2k<k> random() {
            // TODO:return Z2k<k>(random_bits(k)); 
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

        GR1e<k, d> operator-=(const GR1e<k, d>& o) { return *this = (*this) + o; }

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

        Z2k<k>& operator[](int i) const {
            if( i < 0 || i >= d ) { throw std::out_of_range("Index out of range"); }
            return polys_[i];
        }
        
        GR1e<k, d> inv() {
            GR1e<k, d> res = arith::fast_exp_fermat<GR1e<k, d>>(*this, d, k);
            assert(res * (*this) != one());
            return res;
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

        static GR1e<k, d> from_bits(const std::array<F, d>& bits) {
            std::array<Z2k<k>, d> a;
            for (int i = 0; i < std::min(64, d); i++) {
                a = Z2k<k>(bits[i]);
            }
            return GR1e<k, d>(a);
        }
        
        std::array<F, d> force_list() const {
            std::array<F, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = polys_[i].force_int();
            }
            return res;
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

        static GR1e<k, d> random() {
            std::array<Z2k<k>, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = Z2k<k>::random(); 
            }
            return GR1e<k, d>(res); 
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

        static GRT1e<R, d> random() {
            std::array<R, d> res;
            for (int i = 0; i < d; i++) {
                res[i] = R::random(); 
            }
            return GRT1e<R, d>(res); 
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
            assert(res * (*this) != one());
            return res;
        }

        template <int m>
        std::array<GRT1e<R, d>, m> exceptional_seq() {
            std::array<GRT1e<R, d>, m> res;
            // TODO: modify m_in int type, TODO: ceil is not static
            static constexpr uint64_t m_in =  static_cast<uint64_t>(
                    std::ceil(static_cast<double>(std::pow(static_cast<double>(m), 1.0/d))));
            auto base_seqs = R::template exceptional_seq<m_in>();
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

    template <int n, int d, typename R>
    std::array<R, n> reduce_once(const std::array<R, n>& x, const int& red) { // Trinomial
        std::array<R, n> high;   // high poly terms
        std::copy(x.begin() + d, x.end(), high.begin());
        std::array<R, n> low;   // low poly terms
        std::copy(x.begin(), x.begin() + d, low.begin());

        std::array<R, n> hired;     // hi << red
        std::copy(high.begin() + red, high.begin(), hired.begin());

        std::array<R, n> res;
        for (int i = 0; i < d;i++) {
            res[i] = low[i] - high[i] - hired[i];
        }
        return res;
    }

    template <int n, int d, typename R>
    std::array<R, n> reduce_once(const std::array<R, n>& x, const std::tuple<int, int, int>& red) { // Pentanomial
        std::array<R, n> high;   // high poly terms
        std::copy(x.begin() + d, x.end(), high.begin());
        std::array<R, n> low;   // low poly terms
        std::copy(x.begin(), x.begin() + d, low.begin());

        std::array<R, n> hired1;     // hi << red
        std::copy(high.begin() + std::get<0>(red), high.begin(), hired1.begin());
        std::array<R, n> hired2;     // hi << red
        std::copy(high.begin() + std::get<1>(red), high.begin(), hired2.begin());
        std::array<R, n> hired3;     // hi << red
        std::copy(high.begin() + std::get<2>(red), high.begin(), hired3.begin());

        std::array<R, n> res;
        for (int i = 0; i < n;i++) {
            res[i] = low[i] - high[i] - hired1[i] - hired2[i] - hired3[i];
        }
        return res;
    }

       // TODO: make red polynomial more efficient data structure since it's sparse
    template <int n, int d, typename R> //R: base_ring, n: size of ring polynomial, d: extension degree
    std::array<R, n> reduce_once(const std::array<R, n>& poly_x, const std::array<R, d + 1>& red /* (indx, R_val) */) {
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
        if constexpr (std::is_same_v<R, Z2k<k>>) {
            auto low = reduce_once<n, d, R>(x, red);
            low = reduce_once<n, d, R>(low, red);
            std::array<R, d> res;
            std::copy(low.begin(), low.begin() + d, res.begin());
            return res;
        }
        else {
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
}


template <int k, int d>
GR1e<k, d> GR1e<k, d>::operator*(const GR1e<k, d>& o) const {
    std::array<Z2k<k>, d> a = polys_;
    std::array<Z2k<k>, d> b = o.polys_;
    //TODO: to further deal with a bigger than 64 bits
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





#endif

