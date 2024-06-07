# This file generates the primitive polynomial of towering of GR via the towering of finite fields

D0_LIFT_DEG = [i for i in range(2,4)]
#TODO: 不支持d0 = 1。
LIFT_DEG = [[i for i in range(2,5)], [i for i in range(2,5)], [i for i in range(2,5)]]
ALPHABET = ['α','β','γ']
MAX_LAYER = 3   # = 4 liftings at most
# GR(BR,k)

modulus = {}

for base_d0 in D0_LIFT_DEG:
    def recurse_build(layer, prev_R, prev_degs):
        if layer >= MAX_LAYER:
            return
        def find_irreducible(R1, d0):
            while true:
                r1_ele = R1.random_element(degree=d0)
                if r1_ele.is_irreducible():
                    F2 = R1.quo(r1_ele)
                    R2 = F2[ALPHABET[layer - 1]]     
                    return F2, R2         
        for lift_d1 in LIFT_DEG[layer]:
            R1 = prev_R
            if layer == 1:  
                F2 = R1.quo(R1.irreducible_element(lift_d1, algorithm="first_lexicographic"))
                R2 = F2[ALPHABET[layer - 1]]
            else:
                F2, R2 = find_irreducible(R1, lift_d1)      
            def modulus_to_list(moduli, layer):
                if layer == 0:  #单独判断base_d0 = 1，因为int没有list.
                    return moduli if not hasattr(moduli, 'list') else moduli.list()
                return [modulus_to_list(term, layer - 1) for term in moduli.list()]
            modulus[tuple(prev_degs) + (lift_d1,)] = modulus_to_list(F2.modulus(), layer)
            #print(prev_degs + [lift_d1])
            #print(F2.modulus())
            recurse_build(layer + 1, R2, prev_degs + [lift_d1])
    F1 = GF(2^base_d0, 'ζ', modulus="minimal_weight")
    R1 = F1['δ']
    prev_degs = [base_d0]
    recurse_build(1, R1, prev_degs)
    

# print(modulus)
#TODO: test the correctness
'''def test():
    for deg in modulus.keys():
        print(deg) 
'''

textual_embeddings = []
impl_embeddings_decl = []
impl_embeddings_def = []
for k, v in modulus.items():
    textual_embeddings.append(
        f"template <> inline const GR<{k[1]}> lift_v<{k[0]}, {k[1]}>[{k[0]}] = "
        + "{%s};" % ",".join("GR<%d>{%du}" % (k[1], x) for x in v))


with open("grmodtables.h", "w") as f:
    f.write("""
// This file was automatically generated, changes may be overwritten
#pragma once
#include <cstdint>
#include "gring.h"

namespace grmodtables {
    template <int k1, int k2> extern const GR<k2> lift_v[k1];
    %s
} // namespace grmodtables
""" % "\n    ".join(textual_embeddings))




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