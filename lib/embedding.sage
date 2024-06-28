EMBEDDINGS = {}
BASE_K = [i for i in range(1, 65)]
LIFT_D0 = [i for i in range(2, 33)]
LIFT_D1 = 32

def modulus_poly(k):
    return GF(2^k, 'ζ', modulus="minimal_weight").modulus()

#TODO: delete linear time solution。。
def lift2k(k, Rs_moduli, Rl_moduli, r1, algorithm = 'log'):  #k, the Rsmall's polynomial, Rlarge's reduction polynomial, the previous roots
    Rbase = Zmod(2^k)
    Rlarge = Rbase.extension(Rl_moduli)
    f = Rlarge['ζ'](Rs_moduli.list()) 
    PRb = PolynomialRing(Rbase, 'z', Rlarge.degree())
    PRl = PRb.extension(Rlarge.modulus())
    if k > 2:
        r1 = r1.lift()  # lift it to Rlarge elements
    if algorithm == 'linear':
        r2_symbolic = PRl([PRb.gens()[i] * 2 + Rlarge(r1)[i] for i in range(Rlarge.degree())])
    else:
        r2_symbolic = PRl([PRb.gens()[i] * 2^(k // 2) + Rlarge(r1)[i] for i in range(Rlarge.degree())])
    f_r2 = f.change_ring(PRb)(r2_symbolic)
    coeff = [{PRb.gens()[i]:1 if i == j else 0 for i in range(Rlarge.degree())} for j in range(Rlarge.degree())]
    if algorithm == 'linear':
        M = Matrix(Zmod(2^(k - 1)), [[ZZ(c.coefficient(dic)) // 2 for dic in coeff] for c in f_r2.list()])
        b = vector(Zmod(2^(k - 1)), [ZZ((-1) * c.constant_coefficient()) // 2 for c in f_r2.list()])
    else:
        M = Matrix(Zmod(2^(k // 2)), [[ZZ(c.coefficient(dic)) // 2^(k // 2) for dic in coeff] for c in f_r2.list()])
        b = vector(Zmod(2^(k // 2)), [ZZ((-1) * c.constant_coefficient()) // 2^(k // 2) for c in f_r2.list()])
    z_assignment = M.solve_right(b) 
    r2 = Rlarge([x.subs({g:ZZ(a) for g, a in zip(PRb.gens(), z_assignment)}) for x in r2_symbolic.list()])
    assert f(r2) == 0
    return r2


#OM and OlogM solution
def lift(k0, d_small, d_large, algorithm = 'log'):
    def first_root(d_small, d_large):
        Rbase = Zmod(2)
        Rlarge = Rbase.extension(modulus_poly(d_large))
        f = Rlarge['ζ'](modulus_poly(d_small).list()) 
        F = GF(2).extension(Rlarge.modulus(), name='x')
        r1 = F['ζ']([F(x.lift()) for x in f.list()]).roots()[0][0]  # lift是把quotient ring的元素转换为Zmod再放到field上 #TODO: use any_root()
        return r1
    msmall, mlarge = modulus_poly(d_small), modulus_poly(d_large)
    r2 = lift2k(2, msmall, mlarge, first_root(d_small, d_large))
    if algorithm == 'linear':
        for k in range(3, k0 + 1):
            r2 = lift2k(k, msmall, mlarge, r2, algorithm = 'linear')
        r2_powers = [(r2^i).list() for i in range(d_small)]   #from x^0 to x^k
    else:
        k = 2
        while k <= k0:
            k *= 2
            r2 = lift2k(k, msmall, mlarge, r2)
            #print("r" + str(k) + " = ", r2)
        #r2 = [Zmod(1<<k0)(x) for x in r2.list()]  #truncate
        r2_truncate = Zmod(1<<k0).extension(mlarge)(r2.list())
        r2_powers = [(r2_truncate^i).list() for i in range(d_small)] 
    return r2_powers

def genlifttables():
    for k0 in BASE_K:
        for d_small in LIFT_D0:
            extension_factor = 2
            while d_small * extension_factor <= LIFT_D1:
                d_large = d_small * extension_factor
                EMBEDDINGS[(k0, d_small, d_large)] = lift(k0, d_small, d_large)
                extension_factor += 1

def genlifttables_efficient():
    k0 = BASE_K[-1]  #max of base_k
    for d_small in LIFT_D0:
        extension_factor = 2
        while d_small * extension_factor <= LIFT_D1:
            d_large = d_small * extension_factor
            EMBEDDINGS[(k0, d_small, d_large)] = lift(k0, d_small, d_large)
            extension_factor += 1
    #print([ k for k,v in EMBEDDINGS.items()])      
    for i in BASE_K[:-1]:
        for d_small in LIFT_D0:
            extension_factor = 2
            while d_small * extension_factor <= LIFT_D1:
                d_large = d_small * extension_factor
                r2 = EMBEDDINGS[(k0, d_small, d_large)]   #truncate instead of recomputing
                EMBEDDINGS[(i, d_small, d_large)]  = [[Zmod(1 << i).extension(modulus_poly(d_large))(r) for r in s] for s in r2]
                extension_factor += 1

    

genlifttables_efficient()

print("------------------------------------generate completed-----------------------------------")
#print(EMBEDDINGS)

textual_embeddings = []
for key, v in EMBEDDINGS.items():
    k, d0, d1 = key # k[1] terms, 一个term里面有k[2]个int
    r_powers = [f"GR1e<{k}, {d1}>({{{', '.join(f'{x}u' for x in s)}}})" for s in v]  #{{是用来转义{
    textual_embeddings.append(f"template <> inline const GR1e<{k}, {d1}> lift_v<{k}, {d0}, {d1}>[{d0}] = {{{', '.join(r_powers)}}};")

with open("brlifttables.h", "w") as f:
    f.write("""
// This file was automatically generated, changes may be overwritten
#pragma once
#include <cstdint>
// Only to keep everything looking nice if you somehow would include the file directly; it's circular otherwise
#include "gring.h"

namespace brlifttables {
    template <int k, int d0, int d1> extern const GR1e<k, d1> lift_v[d0];
    %s
} // namespace brlifttables
""" % "\n    ".join(textual_embeddings))

print("------------------------------------writefile completed----------------------------------")

'''
def test():    
    k0 = 16
    d_small = 3
    d_large = 9
    import time
    start_time = time.time()
    r1 = lift(k0, d_small, d_large, algorithm = 'linear')
    end_time = time.time()
    print("linear time is ", end_time - start_time)
    start_time = time.time()
    r2 = lift(k0, d_small, d_large)
    end_time = time.time()
    print("log time is ", end_time - start_time)
    assert(r1 == r2)
    print("passed")

test()
'''
