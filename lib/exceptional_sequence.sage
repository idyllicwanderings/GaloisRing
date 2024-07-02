import itertools as it

D0_DEG = [i for i in range(1, 3)]
K_DEG = [i for i in range(1, 4)]

#TODO: 
# 1. no towering for now

EX = {}
for base_k in K_DEG:
    for ext_d0 in D0_DEG:
        EX[(base_k, ext_d0)] = [list(i) for i in it.product(range(2), repeat=ext_d0)]
        '''Z = Zmod(2^base_k)['x']
        R.<x> = Z.quotient(GF(2^ext_d0, name='x', modulus="minimal_weight").modulus())
        for s in EX[(base_k, ext_d0)]:
            a = R(s)
            if a == 0:
                continue
            print("k: ", base_k)
            print("d0: ", ext_d0)
            print("s: ", s)
            print("a: ", a)
            RR.<y> = R[]
            inv = (a*y - 1).roots(multiplicities=False)[0].list()

            INV[(base_k, ext_d0)].append(inv)
        '''

print("------------------------------------generate completed-----------------------------------")


# GR1e<k, d>
textual_sequence = [] 
for key, seq in EX.items():
    k, d1 = key
    seq_poly = [f"GR1e<{k}, {d1}>({{{', '.join(f'{x}u' for x in s)}}})" for s in seq] 
    #inv_poly = [f"GR1e<{k}, {d1}>({{{', '.join(f'{x}u' for x in s)}}})" for s in INV[key]] 
    textual_sequence.append(f"template <> inline const GR1e<{k}, {d1}> exseq_v<{k}, {d1}, {d1}>[{d1}] = {{{', '.join(seq_poly)}}};")
    #textual_sequence.append(f"template <> inline const GR1e<{k}, {d1}> exinv_v<{k}, {d1}, {d1}>[{d1}] = {{{', '.join(inv_poly)}}};")

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
