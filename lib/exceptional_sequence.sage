from itertools import permutations 

EX = {}
INV = {}
D0_DEG = [i for i in range(33)]
#TODO: 
# 1. no towering for now
# 2. calculate and store inverse
# 3. not invertible

for ext_d0 in D0_LIFT_DEG:
    EX[ext_d0] = list(it.product(range(2), repeat = ext_d0))

print("------------------------------------generate completed-----------------------------------")


# GR1e<k, d>
textual_sequence = [] 
for d1, seq in EX.terms():
    seq_poly = [f"GR1e<{k}, {d1}>({{{', '.join(f'{x}u' for x in s)}}})" for s in seq] 
    inv_poly = [f"GR1e<{k}, {d1}>({{{', '.join(f'{x}u' for x in s)}}})" for s in INV[d1]] 
    textual_embeddings.append(f"template <> inline const GR1e<{k}, {d1}> exseq_v<{k}, {d1}, {d1}>[{d1}] = {{{', '.join(seq_poly)}}};")
    textual_embeddings.append(f"template <> inline const GR1e<{k}, {d1}> exinv_v<{k}, {d1}, {d1}>[{d1}] = {{{', '.join(inv_poly)}}};")

    textual_sequence.append("
    n = len(data)  
    prev_tem = f"GR1E<k, {n}>"
    "GR1E<k, {}>({{{}}})".format(len(data), ', '.join(f"{x}u" for x in data))
    inner_strs = [enum_build(i, layer + 1) for i in data]
    inner_tem = "GR1E<k, {n}>" if layer == 1 else f"GR< {prev_tem}, {len(data[0])}>"
    prev_tem = inner_tem
    return f"GR< {inner_tem}, {n}>({{{', '.join(inner_strs)}}})"
    textual_sequence.append(f"template <int k> inline const moduli<k, " +", ".join(str(x) for x in deg) + "> = " + enum_build(moduli, layer = len(deg)) + f";")


with open("exseqtables.h", "w") as f:
    f.write("""
// This file was automatically generated, changes may be overwritten
#pragma once
#include <cstdint>
// Only to keep everything looking nice if you somehow would include the file directly; it's circular otherwise
#include "gring.h"

namespace exseqtables {
    template <int k, int d0> extern const GR1e<k, d0> exseq_v[2^d0];   
    %s
} // namespace exseqtables
""" % "\n    ".join(textual_sequence))

print("------------------------------------writefile completed----------------------------------")