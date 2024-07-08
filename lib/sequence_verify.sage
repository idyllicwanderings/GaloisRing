import itertools as it
import math
ALPHABET = ['β','γ','δ','ε','ζ','η','θ','ι','κ','λ','μ','ν','ξ','ο','π','ρ','σ','τ','υ','φ','χ','ψ','ω']

'''
def build_gr_towers(k, d_list, modulus_list = None):
    Z2K = Zmod(2^k)  #construct base ring GF(2^k, d)
    layer = 0
    moduli = GF(2^d_list[layer], name="z", modulus="minimal_weight").modulus()
    R1.<α> = Z2K[]
    GR_list = [R1.quotient(moduli)]

    def build_modulus(GR_list, mod_list, layer):
        if layer == 0:
            return mod_list 
        return GR_list[layer][ALPHABET[layer]]([build_modulus(GR_list, mod.lift(), layer - 1) for mod in mod_list])


    F1 = GF(2^d_list[layer], name="z", modulus="minimal_weight")  #find polynomial
    RX.<ζ> = F1['ζ']
    def find_irreducible(R1, d0):
        while true: 
            r1_ele = R1.gen()^d0 + R1.random_element(degree=d0//2) 
            if r1_ele.is_primitive():
                F2 = R1.quo(r1_ele)
                R2 = F2[ALPHABET[layer - 1]]     
                return R2, r1_ele    
    while (layer < len(d_list) - 1):
        layer += 1
        RX, moduli = find_irreducible(RX, d_list[layer])
        print("moduli: ", moduli)
        GRX = GR_list[layer - 1][ALPHABET[layer - 1]].quotient(build_modulus(GR_list, moduli, layer - 1))
        GR_list.append(GRX)
    return GR_list[-1]
'''

# fix the modulus as lists
def build_gr_towers(k, d_list, modulus_list = None):
    Z2K = Zmod(2^k)  #construct base ring GF(2^k, d)
    layer = 0
    moduli = GF(2^d_list[layer], name="z", modulus="minimal_weight").modulus()
    R1.<α> = Z2K[]
    GR_list = [R1.quotient(moduli)]
    
    #GF(2^k, d) 
    def modulus_to_list(moduli, layer):
        if layer == 0:  
            return moduli if not hasattr(moduli, 'list') else moduli.list() 
        return [modulus_to_list(term, layer - 1) for term in moduli.list()]
    
    def build_modulus(GR_list, mod_list, layer):
        if layer == 0:
            print("mod_list is,", mod_list)
            return mod_list 
        return GR_list[layer - 1][ALPHABET[layer]]([build_modulus(GR_list, mod, layer - 1) for mod in mod_list])


    F1 = GF(2^d_list[layer], name="z", modulus="minimal_weight")  #find polynomial
    RX.<ζ> = F1['ζ']
    def find_irreducible(R1, d0):
        while true: 
            r1_ele = R1.gen()^d0 + R1.random_element(degree=d0//2) 
            if r1_ele.is_irreducible():
                F2 = R1.quo(r1_ele)
                R2 = F2[ALPHABET[layer - 1]]     
                return R2, r1_ele    
    while (layer < len(d_list) - 1):
        layer += 1
        RX, moduli = find_irreducible(RX, d_list[layer])
        moduli = modulus_to_list(moduli, layer)
        print("moduli: ", moduli)
        GRX = GR_list[layer - 1][ALPHABET[layer - 1]].quotient(build_modulus(GR_list, moduli, layer))
        GR_list.append(GRX)
    return GR_list[-1]

'''
d_list = [5, 3, 4]
GR = build_gr_towers(4, d_list)
print(GR)
print("GR type: ", parent(GR))
print("d_list: ", d_list)
print(GR.random_element())
'''

def test_one_towering():
    d = 5
    k = 4
    GR = build_gr_towers(4, [5])
    print("k = ", k)
    print("d_list: ", [5])
    print(GR)
    print("GR type: ", parent(GR))
    sequence = [list(i) for i in it.product(range(2), repeat=d)]
    for i in sequence:
        a = GR(i)
        if a == 0:
            continue
        inv = a^((2^d - 1) * 2^(k - 1) - 1)
        print("a: ", a)
        print("inv: ", inv)
        print("a * inv: ", a * inv)
        assert inv * a == 1
    print(GR.random_element())

#test_one_towering() #PASS


def test_mul_towering():
    d = [2, 3, 2]
    k = 3
    print("k = ", k)
    print("d_list: ", d)
    GR = build_gr_towers(k, d)
    print(GR)
    print("GR type: ", parent(GR))

    def all_permutations(d_list):
        per = [list(i) for i in it.product(range(2), repeat = d_list[0])]
        for i in range(1, len(d_list)):
            per = [list(i) for i in it.product(per, repeat = d_list[i])]
        return per

    seq = all_permutations(d)    
    print("len(seq): ", len(seq))
    d_prime = math.prod(d)
    j = 0
    for i in seq:
        a = GR(i)
        if a == 0:
            continue
        inv = a^((2^d_prime - 1) * 2^(k - 1) - 1)
        j += 1
        if j % 1000 == 0:
            print("now ", j)
        '''
        print("a: ", a)
        print("inv: ", inv)
        print("a * inv: ", a * inv)
        '''
        assert inv * a == 1
    '''
    for i in sequence:
        a = GR(i)
        if a == 0:
            continue
        inv = a^((2^d - 1) * 2^(k - 1) - 1)
        print("a: ", a)
        print("inv: ", inv)
        print("a * inv: ", a * inv)
        assert inv * a == 1
    print(GR.random_element())
    '''

test_mul_towering() #PASS


#TODO: 写一个生成sequence的函数。写到头文件里。别给我乱auto generate的东西，闭嘴吧你、


with open("sequence.h", "w") as f:
    f.write("""
// This file was automatically generated, changes may be overwritten
#pragma once
#include <cstdint>
// Only to keep everything looking nice if you somehow would include the file directly; it's circular otherwise
#include "gring.h"

namespace exseqtables {
    template <int k, int d0> extern const std::array<Z2k<k>, d0 + 1> moduli;
    template <int k, int d0, int d1> extern const std::array<GR1e<k, d0>, d1 + 1> moduli;
    template <int k, int d0, int d1, int d2> extern const std::array <GRT1e <GR1e<k, d0>, d1>, d2 + 1> moduli;
    template <int k, int d0, int d1, int d2, int d3> extern const std::array< GRT1e <GRT1e <GR1e<k, d1>, d0>, d2>, d3 + 1> moduli;
    template <int k, int d0, int d1, int d2, int d3, int d4> extern const std::array< GRT1e <GRT1e <GRT1e <GR1e<k, d0>, d1>, d2>, d3>, d4 + 1> moduli;
    
    %s
} // namespace exseqtables
""" % "\n    ".join(textual_towerings))

