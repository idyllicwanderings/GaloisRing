#ifndef __GALOIS_RING_H
#define __GALOIS_RING_H

#include <emmintrin.h>
#include <smmintrin.h>
#include <wmmintrin.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>

#include <glog/logging.h>

#include "random.h"

namespace arith {
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

            int128 operator+(const int128& o) const {
                __m128i packed_sum = _mm_add_epi64(m_val, o.m_val);
                
                __m128i rangeshift = _mm_set1_epi64x(0x8000000000000000);  // unsigned comparison
                __m128i carry = _mm_cmpgt_epi64(_mm_xor_si128(m_val, rangeshift), 
                                     _mm_xor_si128(packed_sum, rangeshift));
                if (!_mm_test_all_zeros(carry, _mm_set_epi64x(0, -1))) {
                    // add carry bit from lower 64 bits
                    packed_sum = _mm_add_epi64(packed_sum,  _mm_set_epi64x(0, 1));
                }
                return packed_sum;
            }

            int128& operator+=(const int128& o) {
                return *this = *this + o;
            }

            int128 operator*(const int128& o) const {
                // TODO
            }

            int128& operator*=(const int128& o) {
                return *this = *this * o;
            }

            // int128 operator&(const int128& o) const {
            //     return m_val & o.m_val;
            // }
            // int128& operator&=(const int128& o) {
            //     return *this = *this & o;
            // }

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
                return packed_shifted ^ _mm_srli_si128(cross_boundary, 8); 
                // Right shift the mixin by 8 *bytes* to bring high into low
            }

            inline int128& operator>>=(int s) {
                return *this = (*this) >> s;
            }

            inline int128 operator<<(int s) const {
                __m128i packed_shifted = _mm_slli_epi64(m_val, s); // Shift both parts
                __m128i cross_boundary = m_val;
                if (s < 64) {
                    cross_boundary = _mm_srli_epi64(m_val, 64 - s); // MSB of low to become LSB of high
                } else if (s > 64) {
                    cross_boundary = _mm_slli_epi64(m_val, s - 64); // LSB of low to become MSB of high
                }
                return packed_shifted ^ _mm_slli_si128(cross_boundary, 8); // Left shift by 8 bytes to bring low into high
            }
            inline int128& operator<<=(int s) {
                return *this = (*this) << s;
            }

            bool operator==(const int128& o) const {
                const __m128i tmp = m_val ^ o.m_val;
                return _mm_test_all_zeros(tmp, tmp);
            }

            bool operator!=(const int128& o) const {
                return !(*this == o);
            }

            bool operator<(const unsigned long long& other) { 
                // Special case for this, because it's a pain to do with another int128
                return !(
                        _mm_test_all_zeros(m_val, _mm_set_epi64x(-1, 0))                  
                        // if anything is in the top 64 bits, it's definitely bigger
                     || static_cast<unsigned long long>(_mm_cvtsi128_si64(m_val)) >= other 
                        // otherwise, unsigned compare the rest
                     );
            }
 
            bool operator>(const int128& o) const {
                __m128i rangeshift = _mm_set1_epi64x(0x8000000000000000);
                __m128i gt_mask = _mm_cmpgt_epi64(_mm_xor_si128(m_val, rangeshift), 
                                     _mm_xor_si128(o.m_val, rangeshift));
                __m128i eq_mask =  _mm_cmpeq_epi64(m_val, o.m_val);
                
                return (    
                    !_mm_test_all_zeros(gt_mask, _mm_set_epi64x(-1, 0)) // higher bits are zero
                  ||  (!_mm_test_all_zeros(gt_mask, _mm_set_epi64x(0, -1)) 
                    && !_mm_test_all_zeros(eq_mask, _mm_set_epi64x(-1, 0)))
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

 
    inline std::ostream& operator<<(std::ostream& os, const int128& x) {
        return os << std::hex << std::setfill('0') << std::setw(16) << x.high()
            << std::hex << std::setfill('0') << std::setw(16) << x.low();
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

};


/**
 * (BR(k), extension d0)
 * Only supports k = 2 to 64
*/
template<int k>
class BR {
    public:
        // TODO: mask?  what did you do for masking the type
        using Z2K = typename arith::datatype<arith::type_idx<k>()>::type;
    private:
        static_assert(2 <= k && k <= 64, "Unsupported Base Ring Z2K");

    public:
        template <typename T>
        explicit BR<k>(const T& el) : m_val(F(el) & MASK), d0_(1) {}

        explicit BR<k>(F f, bool /*skip mask*/) : m_val(std::move(f)) {}
        
        BR<k>() : val_(0), d0_(1) {}

    public:
        static void reduction() {

        }

        static BR<k> random(PRNG& gen) {
            return BR<k>(detail::random<F>(gen)); // TODO:auto masked?
        }

        BR operator+(const BR& o) const {
            return BR<k>(val_ + other.val_, true);
        }

        BR operator+=(const BR& o) {
            return *this = (*this) + o;
        }

        BR operator-(const BR& o) const {
            return BR<k>(val_ - other.val_, true);
        }

        BR operator-=(const BR& o) {
            return *this = (*this) + o;
        }

        BR operator*(const BR& o) const {
            Z2K a = val_;
            Z2K b = o.val_;

            //TODO
            //return Z2K<k>(detail::extract<Z2K>(detail::reduce<k, F> \
                                         (detail::initial_mult<k>(a, b))));
        }

        BR& operator*=(const BR& o) {
            return *this = (*this) * o;
        }

        bool operator==(const BR& o) const {
            return (val_ == o.val_ && d0_ == o.d0_);
        }

        bool operator!=(const BR& o) const {
            return (val_ != o.val_ || d0_ != o.d0_);
        }

    private:
        Z2K val_;
        const uint16_t d0_;
        reduction_poly_

};
/**
 * (GR(BR, k))
*/
template <int k, BR<k>, int d>
class GR {
    public:
        
        // static GF2k<k> random(PRNG& gen) {
        //     return GF2k<k>(detail::random<F>(gen)); // auto masked
        // }

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

