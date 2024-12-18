#include <immintrin.h>
#include <iostream>

void print_m128i(__m128i val)
{
    long long int *ptr = (long long int *)&val;
    std::cout << "High: " << std::hex << ptr[1] << ", Low: " << ptr[0] << std::endl;
}

class int128
{
public:
    __m128i m_val;

    int128(__m128i val) : m_val(val) {}

    // Multiplication operator for int128
    int128 operator*(const int128 &o) const
    {
        __m128i a = m_val;
        __m128i b = o.m_val;

        // Extract 64-bit parts
        __m128i a_lo = _mm_unpacklo_epi64(a, _mm_setzero_si128());
        __m128i a_hi = _mm_unpackhi_epi64(a, _mm_setzero_si128());
        __m128i b_lo = _mm_unpacklo_epi64(b, _mm_setzero_si128());
        __m128i b_hi = _mm_unpackhi_epi64(b, _mm_setzero_si128());

        // Perform 64-bit multiplication for low and high parts
        __m128i prod_ll = _mm_mul_epu32(a_lo, b_lo); // Low * Low
        __m128i prod_lh = _mm_mul_epu32(a_lo, b_hi); // Low * High
        __m128i prod_hl = _mm_mul_epu32(a_hi, b_lo); // High * Low
        __m128i prod_hh = _mm_mul_epu32(a_hi, b_hi); // High * High

        // Combine products into 128-bit result
        __m128i mid = _mm_add_epi64(prod_lh, prod_hl);

        // Correct carry bits
        __m128i mid_lo = _mm_slli_epi64(mid, 32); // Shift lower parts
        __m128i mid_hi = _mm_srli_epi64(mid, 32); // Shift higher parts

        __m128i result_lo = _mm_add_epi64(prod_ll, mid_lo);

        // Detect carry from the lower 64-bit addition
        __m128i carry_mask = _mm_set1_epi64x(0xFFFFFFFFFFFFFFFF);
        __m128i carry = _mm_cmpeq_epi64(_mm_and_si128(result_lo, carry_mask), _mm_setzero_si128());
        carry = _mm_srli_epi64(carry, 63); // Extract carry as a bit

        __m128i result_hi = _mm_add_epi64(prod_hh, mid_hi);
        result_hi = _mm_add_epi64(result_hi, carry);

        __m128i result = _mm_unpacklo_epi64(result_lo, result_hi);

        return int128(result);
    }

    // g++ int_demo.cpp -std=c++20 -msse4.2 -o int_demo
    int128 operator+(const int128 &o) const
    {
        __m128i a = m_val;
        __m128i b = o.m_val;

        // Extract low and high 64-bit parts of both operands
        __m128i a_lo = _mm_unpacklo_epi64(a, _mm_setzero_si128());
        __m128i a_hi = _mm_unpackhi_epi64(a, _mm_setzero_si128());
        __m128i b_lo = _mm_unpacklo_epi64(b, _mm_setzero_si128());
        __m128i b_hi = _mm_unpackhi_epi64(b, _mm_setzero_si128());

        // Perform 64-bit addition on the lower parts
        __m128i sum_lo = _mm_add_epi64(a_lo, b_lo);

        // Detect carry from the lower 64-bit addition
        __m128i carry_mask = _mm_set1_epi64x(0xFFFFFFFFFFFFFFFF);
        __m128i carry = _mm_cmpeq_epi64(_mm_and_si128(sum_lo, carry_mask), _mm_setzero_si128());
        carry = _mm_srli_epi64(carry, 63); // Extract carry as a bit

        // Perform 64-bit addition on the higher parts with carry
        __m128i sum_hi = _mm_add_epi64(a_hi, b_hi);
        sum_hi = _mm_add_epi64(sum_hi, carry);

        // Combine low and high parts back into a single __m128i
        __m128i result = _mm_unpacklo_epi64(sum_lo, sum_hi);

        return int128(result);
    }

    int128 operator>>(int s) const
    {
        __m128i packed_shifted = _mm_srli_epi64(m_val, s); // Shift both parts
        __m128i cross_boundary = m_val;
        if (s < 64)
        {
            cross_boundary = _mm_slli_epi64(m_val, 64 - s); // LSB of high to become MSB of low
        }
        else if (s > 64)
        {
            cross_boundary = _mm_srli_epi64(m_val, s - 64); // MSB of high to become LSB of low
        }
        return packed_shifted ^ _mm_srli_si128(cross_boundary, 8); // Right shift the mixin by 8 *bytes* to bring high into low
    }

    int128 &operator>>=(int s)
    {
        return *this = (*this) >> s;
    }

    int128 operator<<(int s) const
    {
        __m128i packed_shifted = _mm_slli_epi64(m_val, s); // Shift both parts
        __m128i cross_boundary = m_val;
        if (s < 64)
        {
            cross_boundary = _mm_srli_epi64(m_val, 64 - s); // MSB of low to become LSB of high
        }
        else if (s > 64)
        {
            cross_boundary = _mm_slli_epi64(m_val, s - 64); // LSB of low to become MSB of high
        }
        return packed_shifted ^ _mm_slli_si128(cross_boundary, 8); // Left shift by 8 bytes to bring low into high
    }

    int128 &operator<<=(int s)
    {
        return *this = (*this) << s;
    }

    bool operator==(const int128 &o) const
    {
        const __m128i tmp = m_val ^ o.m_val;
        return _mm_test_all_zeros(tmp, tmp);
    }
    
    bool operator!=(const int128 &o) const
    {
        return !(*this == o);
    }

    bool operator<(const unsigned long long &other)
    { // Special case for this, because it's a pain to do with another int128
        return !(
            _mm_test_all_zeros(m_val, _mm_set_epi64x(-1, 0))                      // if anything is in the top 64 bits, it's definitely bigger
            || static_cast<unsigned long long>(_mm_cvtsi128_si64(m_val)) >= other // otherwise, unsigned compare the rest
        );
    }

    __m128i value() const
    {
        return m_val;
    }

    void print() const
    {
        long long int *ptr = (long long int *)&m_val;
        std::cout << "High: " << std::hex << ptr[1] << ", Low: " << ptr[0] << std::endl;
    }
};

int main()
{
    // __m128i a_val = _mm_set_epi64x(0x0000000000000020, 0x0000000000000001);
    // __m128i b_val = _mm_set_epi64x(0x0000000000000003, 0x0000000000000005);
    // //__m128i a_val = _mm_set_epi64x(0x123456789ABCDEF0, 0x0FEDCBA987654321);
    // //__m128i b_val = _mm_set_epi64x(0x23456789ABCDEF01, 0x1FEDCBA987654322);

    // int128 a(a_val);
    // int128 b(b_val);

    // //int128 result = a * b;
    // //print_m128i(result.m_val);

    // int128 sum = a + b;
    // print_m128i(sum.m_val);

    // // Case 1: Simple addition with no carry
    // __m128i a_val1 = _mm_set_epi64x(0x0000000000000000, 0x00000000FFFFFFFF); // Low: 0xFFFFFFFF
    // __m128i b_val1 = _mm_set_epi64x(0x0000000000000000, 0x0000000000000001); // Low: 0x1
    // int128 a1(a_val1), b1(b_val1);
    // int128 sum1 = a1 + b1;
    // std::cout << "Case 1: No carry\n";
    // print_m128i(sum1.value());

    // // Case 2: Addition with carry from low to high
    // __m128i a_val2 = _mm_set_epi64x(0x0000000000000001, 0xFFFFFFFFFFFFFFFF); // Low: 0xFFFFFFFFFFFFFFFF
    // __m128i b_val2 = _mm_set_epi64x(0x0000000000000001, 0x0000000000000001); // Low: 0x1
    // int128 a2(a_val2), b2(b_val2);
    // int128 sum2 = a2 + b2;
    // std::cout << "Case 2: Carry from low to high\n";
    // print_m128i(sum2.value());

    // // Case 3: Overflow both low and high
    // __m128i a_val3 = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF); // Low and High: 0xFFFFFFFFFFFFFFFF
    // __m128i b_val3 = _mm_set_epi64x(0x0000000000000001, 0x0000000000000001); // Low and High: 0x1
    // int128 a3(a_val3), b3(b_val3);
    // int128 sum3 = a3 + b3;
    // std::cout << "Case 3: Full overflow\n";
    // print_m128i(sum3.value());

    std::cout << "\n\n\nMultiplication\n";

    // Test case 1: Normal multiplication without overflow
    __m128i a_val1 = _mm_set_epi64x(0x0, 0x0000000000000005); // 5
    __m128i b_val1 = _mm_set_epi64x(0x0, 0x0000000000000003); // 3
    int128 a1(a_val1);
    int128 b1(b_val1);
    int128 result1 = a1 * b1;
    std::cout << "Case 1: No overflow" << std::endl;
    print_m128i(result1.value());

    // Test case 2: Multiplication with carry from low to high
    __m128i a_val2 = _mm_set_epi64x(0x0, 0xFFFFFFFF00000000); // Large low part
    __m128i b_val2 = _mm_set_epi64x(0x0, 0x0000000100000000); // Multiplies into high part
    int128 a2(a_val2);
    int128 b2(b_val2);
    int128 result2 = a2 * b2;
    std::cout << "Case 2: Carry from low to high" << std::endl;
    print_m128i(result2.value());

    // Test case 3: Full overflow
    __m128i a_val3 = _mm_set_epi64x(0x0000000100000000, 0x0000000000000000); // Very large high part
    __m128i b_val3 = _mm_set_epi64x(0x0000000100000000, 0x0000000000000000); // Another large high part
    int128 a3(a_val3);
    int128 b3(b_val3);
    int128 result3 = a3 * b3;
    std::cout << "Case 3: Full overflow" << std::endl;
    print_m128i(result3.value());

    // Test case 4: Mixed high and low part multiplication
    __m128i a_val4 = _mm_set_epi64x(0x0000000000000001, 0x00000000FFFFFFFF); // High and low parts
    __m128i b_val4 = _mm_set_epi64x(0x0000000000000001, 0x00000000FFFFFFFF); // High and low parts
    int128 a4(a_val4);
    int128 b4(b_val4);
    int128 result4 = a4 * b4;
    std::cout << "Case 4: Mixed high and low multiplication" << std::endl;
    print_m128i(result4.value());

    return 0;
}