
// This file was automatically generated, changes may be overwritten
#pragma once
#include <cstdint>
// Only to keep everything looking nice if you somehow would include the file directly; it's circular otherwise
#include "gring.h"

namespace exseqtables {
    template <int k, int d0> extern const GR1e<k, d0> exseq_v[2^d0];   
    template <> inline const GR1e<1, 1> exseq_v<1, 1, 1>[1] = {GR1e<1, 1>({0u}), GR1e<1, 1>({1u})};
    template <> inline const GR1e<1, 1> exinv_v<1, 1, 1>[1] = {GR1e<1, 1>({1u})};
    template <> inline const GR1e<1, 2> exseq_v<1, 2, 2>[2] = {GR1e<1, 2>({0u, 0u}), GR1e<1, 2>({0u, 1u}), GR1e<1, 2>({1u, 0u}), GR1e<1, 2>({1u, 1u})};
    template <> inline const GR1e<1, 2> exinv_v<1, 2, 2>[2] = {GR1e<1, 2>({1u, 1u}), GR1e<1, 2>({1u, 0u}), GR1e<1, 2>({0u, 1u})};
    template <> inline const GR1e<2, 1> exseq_v<2, 1, 1>[1] = {GR1e<2, 1>({0u}), GR1e<2, 1>({1u})};
    template <> inline const GR1e<2, 1> exinv_v<2, 1, 1>[1] = {GR1e<2, 1>({1u})};
    template <> inline const GR1e<2, 2> exseq_v<2, 2, 2>[2] = {GR1e<2, 2>({0u, 0u}), GR1e<2, 2>({0u, 1u}), GR1e<2, 2>({1u, 0u}), GR1e<2, 2>({1u, 1u})};
    template <> inline const GR1e<2, 2> exinv_v<2, 2, 2>[2] = {GR1e<2, 2>({3u, 3u}), GR1e<2, 2>({1u, 0u}), GR1e<2, 2>({0u, 3u})};
    template <> inline const GR1e<3, 1> exseq_v<3, 1, 1>[1] = {GR1e<3, 1>({0u}), GR1e<3, 1>({1u})};
    template <> inline const GR1e<3, 1> exinv_v<3, 1, 1>[1] = {GR1e<3, 1>({1u})};
    template <> inline const GR1e<3, 2> exseq_v<3, 2, 2>[2] = {GR1e<3, 2>({0u, 0u}), GR1e<3, 2>({0u, 1u}), GR1e<3, 2>({1u, 0u}), GR1e<3, 2>({1u, 1u})};
    template <> inline const GR1e<3, 2> exinv_v<3, 2, 2>[2] = {GR1e<3, 2>({7u, 7u}), GR1e<3, 2>({1u, 0u}), GR1e<3, 2>({0u, 7u})};
} // namespace exseqtables
