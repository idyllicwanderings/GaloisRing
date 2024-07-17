import itertools as it
import math
import random
ALPHABET = ['β','γ','δ','ε','ζ','η','θ','ι','κ','λ','μ','ν','ξ','ο','π','ρ','σ','τ','υ','φ','χ','ψ','ω']


# fix the modulus as lists
def mkgrtowers(k, d_list, modulus_list = None):
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
        moduli = modulus_to_list(moduli, layer) if modulus_list is None else modulus_list[tuple(k, d_list, )]
        GRX = GR_list[layer - 1][ALPHABET[layer - 1]].quotient(build_modulus(GR_list, moduli, layer))
        GR_list.append(GRX)
    return GR_list[-1]


def element_to_list(a):
    if not hasattr(a, 'list'):
        return a
    return [element_to_list(t) for t in a.list()]

def all_permutations(d_list):
    per = [list(i) for i in it.product(range(2), repeat = d_list[0])]
    for i in range(1, len(d_list)):
        per = [list(i) for i in it.product(per, repeat = d_list[i])]
    return per

def testadd(GR, k, d):  ## test GR1TE addition
    a = GR.random_element()
    b = GR.random_element()
    return [str(element_to_list(a)), str(element_to_list(b)), str(element_to_list(a + b))]

def testsub(GR, k, d):  ## test GR1TE subtraction
    a = GR.random_element()
    b = GR.random_element()
    return [str(element_to_list(a)), str(element_to_list(b)), str(element_to_list(a - b))]

def testmul(GR, k, d):  ## test GR1TE multiply
    a = GR.random_element()
    b = GR.random_element()
    return [str(element_to_list(a)), str(element_to_list(b)), str(element_to_list(a * b))]

def testinv(GR, k, d, seq):  ## test GR1TE inverse
    a = GR(random.choice(seq)) 
    while (a == 0):
        a = GR(random.choice(seq))
    inv_a = a^((2^math.prod(d) - 1) * 2^(k - 1) - 1)
    assert inv_a * a == 1
    return [str(element_to_list(a)), str(element_to_list(inv_a))]



if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: sage genGRTowering.sage n k d1 d2 d3 ... mult|add|sub|inv")

    n = int(sys.argv[1])
    k = int(sys.argv[2])
   
    for i, arg in enumerate(sys.argv[3:], start=3):
        if arg in {"mult", "add", "sub", "inv"}:
            d_end = i
            break
    else:
        d_end = len(sys.argv)
    d = [int(x) for x in sys.argv[3:d_end]]
    operations = set(sys.argv[d_end:])

    GR = mkgrtowers(k, d) 
    seq_permutations = all_permutations(d)

    path = "../../TestVectors/"
    if 'mult' in operations:
        with open(path + "GRToweringMultiplication.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testmul(GR, k, d)) + '\n')
    if 'add' in operations:
        with open(path + "GRToweringAddition.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testadd(GR, k, d)) + '\n')
    if 'sub' in operations:
        with open(path + "GRToweringSubtraction.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testsub(GR, k, d)) + '\n')
    if 'inv' in operations:
        with open(path + "GRToweringInverse.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testinv(GR, k, d, seq_permutations)) + '\n')
