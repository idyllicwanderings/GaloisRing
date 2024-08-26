#include "gring.h"

#define PARTY_NUM 10 

namespace detail {
    template <int d>
    std::array<bool, d> to_bits(std::int64_t value) {
        std::array<bool, d> bits;
        for (std::size_t i = 0; i < d; ++i) {
            bits[i] = (value >> i) & 1;
        }
        return bits;
    }
}




