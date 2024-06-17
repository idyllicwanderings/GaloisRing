# This file generates the primitive polynomial of towering of GR via the towering of finite fields

D0_LIFT_DEG = [i for i in range(2,4)]
#TODO: 不支持d0 = 1。
LIFT_DEG = [[i for i in range(2,5)], [i for i in range(2,5)], [i for i in range(2,5)]]
ALPHABET = ['α','β','γ']
MAX_LAYER = 3   # = 4 liftings at most
# GR(BR,k)

TOWERS = {}

for base_d0 in D0_LIFT_DEG:
    def recurse_build(layer, prev_R, prev_degs):
        if layer >= MAX_LAYER:
            return
        def find_irreducible(R1, d0):
            while true:
                r1_ele = R1.gen()^d0 + R1.random_element(degree=d0//2) #ensures it's monic & sparse
                # r1_ele = R1.random_element(degree=d0 - 1) + R1.gen()^d0  #ensures it's monic
                if r1_ele.is_primitive():
                    F2 = R1.quo(r1_ele)
                    R2 = F2[ALPHABET[layer - 1]]     
                    return F2, R2         
        for lift_d1 in LIFT_DEG[layer]:
            R1 = prev_R
            if layer == 1:  
                # TODO: should we use primitive instead? 思考
                F2 = R1.quo(R1.irreducible_element(lift_d1, algorithm="first_lexicographic"))
                R2 = F2[ALPHABET[layer - 1]]
            else:
                F2, R2 = find_irreducible(R1, lift_d1)      
            TOWERS[tuple(prev_degs) + (lift_d1,)] = modulus_to_list(F2.modulus(), layer)
            recurse_build(layer + 1, R2, prev_degs + [lift_d1])
    F1 = GF(2^base_d0, 'ζ', modulus="minimal_weight")
    R1 = F1['δ']
    prev_degs = [base_d0]
    def modulus_to_list(moduli, layer):
        if layer == 0:  #单独判断base_d0 = 1，因为int没有list.
            return moduli if not hasattr(moduli, 'list') else moduli.list()
        return [modulus_to_list(term, layer - 1) for term in moduli.list()]
    TOWERS[tuple(prev_degs)] = modulus_to_list(F1.modulus(), 0)
    recurse_build(1, R1, prev_degs)
    

#print(modulus)
'''def test():
    for deg in modulus.keys():
        print(deg) 
'''

print("------------------------------------generate completed-----------------------------------")
#print(EMBEDDINGS)

textual_embeddings = []
for d, v in TOWERS.items():  #(d0, d1, d2, ...) GR<GR<BR<k,d0>(s1),d1)(s2),d>(s3)
    def enum_build(d, s, layer):
        if layer == 1:
            return f"GR<{k}, {d1}>({{{', '.join(f'{x}u' for x in s)}}})" for s in v]
        return f"GR<"+ enum_build(s?, layer - 1) + ", {d[layer - 1]}>({s})"
    r_powers = enum_build(d, v, d.size()) 
    textual_embeddings.append(f"template <> inline const BR<{k}, {d1}> lift_v<{k}, {d0}, {d1}>[{d0}] = {{{', '.join(r_powers)}}};")

with open("grtowertables.h", "w") as f:
    f.write("""
// This file was automatically generated, changes may be overwritten
#pragma once
#include <cstdint>
// Only to keep everything looking nice if you somehow would include the file directly; it's circular otherwise
#include "gring.h"

namespace grtowertables {
    template <int k, int d0, int d1> extern const BR<k, d1> lift_v[d0];
    %s
} // namespace grtowertables
""" % "\n    ".join(textual_embeddings))

print("------------------------------------writefile completed----------------------------------")



''' while layer <= MAX_LAYER:
       prev_Rs = {}
        for lift_d1 in LIFT_DEG[layer - 1]:
            def find_irreducible(R1, d0):
                while true:
                    r1_ele = R1.random_element(degree=d0)
                    if r1_ele.is_irreducible():
                        return R1.quo(r1_ele), F2[ALPHABET[layer - 1]]
            if layer == 1:   #TODO: the first uses irreduible_element
                F2 = R1.quo(R1.irreducible_element(lift_d1, algorithm="first_lexicographic"))
                R2 = F2[ALPHABET[layer - 1]]
            else:
                F2, R2 = find_irreducible(R1, lift_d1)        #TODO: the rest uses find_irreducible
            if base_d0 == 1 AND layer == 1:
                modulus[(base_d0, lift_d1)] = [t for t in F2.modulus().list()]
            else:
                modulus[tuple(prev_degs) + (lift_d1,)] = [t.list() for t in F2.modulus().list()]
        F1, R1 = []
        layer += 1
'''