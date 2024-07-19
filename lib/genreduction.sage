# This file generates the primitive polynomial of towering of GR via the towering of finite fields

D0_LIFT_DEG = [i for i in range(2, 8)] #TODO: 不支持d0 = 1。
LIFT_DEG = [[i for i in range(2, 5)], [i for i in range(2, 5)], [i for i in range(2, 5)]]
ALPHABET = ['α','β','γ']
MAX_LAYER = 4   # = 4 liftings at most

TOWERS = {}

def modulus_to_list(moduli, layer):
    if layer == 0:  #单独判断base_d0 = 1，因为int没有list.
        return moduli if not hasattr(moduli, 'list') else moduli.list()
    return [modulus_to_list(term, layer - 1) for term in moduli.list()]

def recurse_build(layer, prev_R, prev_degs, max_layer0):
    if layer >= max_layer0:  
        return
    def find_irreducible(R1, d0):
        while true: #TODO:研究一下比较sparse的polynomial，并且对C++上能有加速的
            r1_ele = R1.gen()^d0 + R1.random_element(degree=d0//2) #ensures it's monic & sparse
            # r1_ele = R1.random_element(degree=d0 - 1) + R1.gen()^d0  #ensures it's monic
            if r1_ele.is_primitive():
                F2 = R1.quo(r1_ele)
                R2 = F2[ALPHABET[layer - 1]]     
                return F2, R2  
                       
    for lift_d1 in LIFT_DEG[layer]:
        R1 = prev_R
        if layer == 1:   # TODO: should we use primitive instead? 思考
            F2 = R1.quo(R1.irreducible_element(lift_d1, algorithm="first_lexicographic"))
            R2 = F2[ALPHABET[layer - 1]]
        else:
            F2, R2 = find_irreducible(R1, lift_d1)      
        TOWERS[tuple(prev_degs) + (lift_d1,)] = modulus_to_list(F2.modulus(), layer)
        recurse_build(layer + 1, R2, prev_degs + [lift_d1], max_layer0)

for base_d0 in D0_LIFT_DEG:
    for max_layer0 in range(MAX_LAYER): 
        F1 = GF(2^base_d0, 'ζ', modulus="minimal_weight")
        R1 = F1['δ']
        prev_degs = [base_d0]
        TOWERS[tuple(prev_degs)] = modulus_to_list(F1.modulus(), 0)
        recurse_build(1, R1, prev_degs, max_layer0)

print("------------------------------------generate completed-----------------------------------")
# TODO: ring check in ZK4Z2K using layers of embedding might lose some 映射结构。。？所以暂时只能做一层。
# template <int k, int d0, int... d1> extern const GR<GR<GR1e<>>???>?? NO.
# or we can write every case of k, d0, d1


textual_towerings = [] #(d0, d1, d2, ...) GR<GR<GR1e<k,d0>(s1),d1)(s2),d>(s3)
textual_declares = [] 
for deg, moduli in TOWERS.items():
    def enum_build(data, deg, layer = 0):
        if layer == 0 and len(deg) == 1:
            return f"Z2k<k>", "{{{}}}".format(", ".join(f"Z2k<k>({x}u)" for x in data))
        if layer == 0:
            return f"GR1e<k, {deg[0]}>", "GR1e<k, {}>({{{}}})".format(deg[0], ", ".join(f"{x}u" for x in data))
        inner_strs = [enum_build(i, deg, layer - 1)[1] for i in data]
        prev_tem = enum_build(data[0], deg, layer - 1)[0] 
        if layer == len(deg) - 1:
            return f"{prev_tem}", f"{{{', '.join(inner_strs)}}}"
        inner_tem = f"GRT1e< {prev_tem}, {len(data[0])}>"
        return inner_tem, f"{inner_tem}({{{', '.join(inner_strs)}}})"
    mtype, mval = enum_build(moduli, deg, len(deg) - 1)
    textual_towerings.append(f"template <int k> inline const std::array<" + mtype + f", {deg[-1] + 1}> "+ "reduction_polynomial<k, " + mtype + f" , {deg[-1]} > = " + mval + f";")
        


with open("grmodtables.h", "w") as f:
    f.write("""
// This file was automatically generated, changes may be overwritten
#pragma once
#include <cstdint>
// Only to keep everything looking nice if you somehow would include the file directly; it's circular otherwise
#include "gring.h"

namespace grmodtables {
    template <int k, typename R, int d0> extern const std::array<R, d0 + 1> reduction_polynomial;
    %s
} // namespace grmodtables
""" % "\n    ".join(textual_towerings))



# TODO
'''
with open("grmodtables.sage", "w") as f:
    f.write("""
# only for storing the towering tables.
TOWERS = %s
""" % "\n    ".join(str(TOWERS)))
'''

print("------------------------------------writefile completed----------------------------------")