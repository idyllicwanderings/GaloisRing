#include <emmintrin.h>
#include <smmintrin.h>
#include <wmmintrin.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>

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

            // 
            inline int128 operator+(const int128& o) const {
                __m128i packed_sum = _mm_add_epi64(m_val, o.m_val);
                __m128i carry = _mm_cmpgt_epi64(m_val, packed_sum);
                if (!_mm_test_all_zeros(carry, _mm_set_epi64x(0, -1))) {
                    // add carry bit from lower 64 bits
                    packed_sum = _mm_add_epi64(packed_sum,  _mm_set_epi64x(0, 1));
                }
                return packed_sum;
            }

            inline int128& operator+=(const int128& o) {
                return *this = *this + o;
            }

            inline int128 operator*(const int128& o) const {
                
            }

            inline int128& operator*=(const int128& o) {
                return *this = *this * o;
            }

            // int128 operator&(const int128& o) const {
            //     return m_val & o.m_val;
            // }
            // int128& operator&=(const int128& o) {
            //     return *this = *this & o;
            // }

            // int128 operator^(const int128& o) const {
            //     return m_val ^ o.m_val;
            // }
            // int128& operator^=(const int128& o) {
            //     return *this = *this ^ o;
            // }

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

    template <typename T>
    T extract(const int128& x) {
        if constexpr(std::is_same_v<T, int128>) {
            return x;
        } else {
            return T(x.low());  //WHY
        }
    }

    class int256 {
        public:
            int256(int128 lo, int128 hi) : m_lo(lo), m_hi(hi) {}

            // TODO: carry bits undealt with
            int256 operator+(const int256& o) const {
                int128 m_c = (m_lo > m_lo + o.m_lo);
                return int256(m_lo + o.m_lo, m_hi + o.m_hi);
            }


            int256 operator&(const int256& o) const {
                return int256(m_lo & o.m_lo, m_hi & o.m_hi);
            }
            int256 operator&(const int128& o) const {
                return int256(m_lo & o.reveal(), 0);
            }

            int256 operator^(const int256& o) const {
                return int256(m_lo ^ o.m_lo, m_hi ^ o.m_hi);
            }

            int256 operator>>(int s) const {
                assert(s < 128);
                int128 lo = (m_lo >> s) ^ (m_hi << (128 - s));
                return int256(lo, m_hi >> s);
            }
            
            int256 operator<<(int s) const {
                assert(s < 128);
                int128 hi = (m_hi << s) ^ (m_lo >> (128 - s));
                return int256(m_lo << s, hi);
            }

            int128 low() const {
                return m_lo;
            }
            int128 high() const {
                return m_hi;
            }

            static int256 make_mask(int k) {
                return {int128::make_mask(std::min(k, 128)), int128::make_mask(std::max(0, k - 128))};
            }
            
        private:
            int128 m_lo, m_hi;
    };


    template <typename...> class BaseRing;

    template <typename T>
    class BaseRing<T> {
        public:
            // TODO: check T's size and k_ is the same;
            BaseRing(int k): k_(k) {;}

        public:
            BaseRing<T> operator+(const T& o) const {
                return val + val;
            }

            BaseRing<T>& operator+=(const T& o) { //TODO: correct or not?
                return *this = *this + o;
            }

            BaseRing<T> operator*(const T& o) const {
                return val * o.val;
            }

            BaseRing<T>& operator*=(const T& o) {
                return *this = *this * o;
            }

        private:


        private:
           T val; 
           const __m128i k_;

    };


    template <typename...> class ExtensionRing;

    template<typename T, typename M>
    class ExtensionRing<T,M>: public BaseRing<typename T> {
        public:
           lift();

           reduction();

           root();

        private:
           using BaseRing<T>::
           basering<typename T> data;
           template <typename T> T data; // TODO: how to meta programming this
    }
}