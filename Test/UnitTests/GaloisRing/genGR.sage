import sys
import itertools as it
import random

def element_to_list(a):
    if not hasattr(a, 'list'):
        return a
    return [element_to_list(t) for t in a.list()]

def mkring(k, d):
    F1 = Zmod(2^k)
    moduli = GF(2^d, name="z", modulus="minimal_weight", repr='int').modulus()
    R1.<β> = F1[]
    F2 = R1.quotient(moduli)
    return F2

def all_permutations(d):
    return [list(i) for i in it.product(range(2), repeat=d)]


def testadd(GR):  ## test GR addition
    a = GR.random_element()
    b = GR.random_element()
    return [str(element_to_list(a)), str(element_to_list(b)), str(element_to_list(a + b))]

def testsub(GR):  ## test GR subtraction
    a = GR.random_element()
    b = GR.random_element()
    return [str(element_to_list(a)), str(element_to_list(b)), str(element_to_list(a - b))]

def testmul(GR):  ## test GR multiply
    a = GR.random_element()
    b = GR.random_element()
    return [str(element_to_list(a)), str(element_to_list(b)), str(element_to_list(a * b))]

def testinv(GR, k, d, seq):  ## test GR inverse
    a = GR(random.choice(seq)) 
    while (a == 0):
        a = GR(random.choice(seq))
    inv_a = a^((2^d - 1) * 2^(k - 1) - 1)
    assert inv_a * a == 1
    return [str(element_to_list(a)), str(element_to_list(inv_a))]


######################################### embedding.sage ######################################
EMBEDDINGS = {}
'''
# for real use:
BASE_K = [i for i in range(1, 65)]
LIFT_D0 = [i for i in range(2, 33)]
'''

#for test 
BASE_K = [i for i in range(2, 7)]
LIFT_D0 = [i for i in range(2, 7)]
LIFT_D1 = 7

def modulus_poly(k):
    return GF(2^k, 'ζ', modulus="minimal_weight").modulus()

def lift2k(k, Rs_moduli, Rl_moduli, r1):  #k, the Rsmall's polynomial, Rlarge's reduction polynomial, the previous roots
    Rbase = Zmod(2^k)
    Rlarge = Rbase.extension(Rl_moduli)
    f = Rlarge['ζ'](Rs_moduli.list()) 
    PRb = PolynomialRing(Rbase, 'z', Rlarge.degree())
    PRl = PRb.extension(Rlarge.modulus())
    if k > 2:
        r1 = r1.lift()  # lift it to Rlarge elements
    r2_symbolic = PRl([PRb.gens()[i] * 2^(k // 2) + Rlarge(r1)[i] for i in range(Rlarge.degree())])
    f_r2 = f.change_ring(PRb)(r2_symbolic)
    coeff = [{PRb.gens()[i]:1 if i == j else 0 for i in range(Rlarge.degree())} for j in range(Rlarge.degree())]
    M = Matrix(Zmod(2^(k // 2)), [[ZZ(c.coefficient(dic)) // 2^(k // 2) for dic in coeff] for c in f_r2.list()])
    b = vector(Zmod(2^(k // 2)), [ZZ((-1) * c.constant_coefficient()) // 2^(k // 2) for c in f_r2.list()])
    z_assignment = M.solve_right(b) 
    r2 = Rlarge([x.subs({g:ZZ(a) for g, a in zip(PRb.gens(), z_assignment)}) for x in r2_symbolic.list()])
    assert f(r2) == 0
    return r2


#OM and OlogM solution
def lift(k0, d_small, d_large):
    def first_root(d_small, d_large):
        Rbase = Zmod(2)
        Rlarge = Rbase.extension(modulus_poly(d_large))
        f = Rlarge['ζ'](modulus_poly(d_small).list()) 
        F = GF(2).extension(Rlarge.modulus(), name='x')
        r1 = F['ζ']([F(x.lift()) for x in f.list()]).roots()[0][0]  # lift是把quotient ring的元素转换为Zmod再放到field上 #TODO: use any_root()
        return r1
    msmall, mlarge = modulus_poly(d_small), modulus_poly(d_large)
    r2 = lift2k(2, msmall, mlarge, first_root(d_small, d_large))
    k = 2
    while k <= k0:
        k *= 2
        r2 = lift2k(k, msmall, mlarge, r2)
    r2_truncate = Zmod(1<<k0).extension(mlarge)(r2.list())
    r2_powers = [(r2_truncate^i).list() for i in range(d_small)] 
    return r2_powers


def lift_gr_ele(k, d_small, d_large, ele):
    r2 = lift(k, d_small, d_large)
    bases =  [[Zmod(1 << k).extension(modulus_poly(d_large))(r) for r in s] for s in r2]
    res = []
    for i in range(d_large):
        x_i = int(sum([bases[j][i] * ele[j] for j in range(d_small)]))
        res.append(x_i)
    return res


######################################### end  ######################################


def testlift(GR1, GR2, k, d0, d1):  ## test GR lifting
    a = GR1.random_element()
    #print("a: ", a)
    a_list = lift_gr_ele(k, d0, d1, a)
    lift_a = GR2(lift_gr_ele(k, d0, d1, a))
    #print("lift_a: ", lift_a)
    return [str(element_to_list(a)), str(element_to_list(lift_a))]

def test_lift_homorphism(GR1, GR2, k, d0, d1):
    a = GR1.random_element()
    #print("a: ", a)
    b = GR1.random_element()
    #print("b: ", b)
    lift_a = GR2(lift_gr_ele(k, d0, d1, a))
    #print("lift_a: ", lift_a)
    lift_b = GR2(lift_gr_ele(k, d0, d1, b))
    #print("lift_b: ", lift_b)
    a_mul_b = a * b
    #print("a_mul_b: ", a_mul_b)
    lift_a_mul_b = GR2(lift_gr_ele(k, d0, d1, a_mul_b))
    #print("lift_a_mul_b: ", lift_a_mul_b)
    assert lift_a * lift_b == lift_a_mul_b
    

if __name__ == "__main__":

    if len(sys.argv) < 3:
        print("Usage: sage genGR.sage testcase_nums k d1 (d2, d2 used for lifting) ... mult|add|sub|inv|lift")
    
    n = int(sys.argv[1])
    k = int(sys.argv[2])
   
    for i, arg in enumerate(sys.argv[3:], start=3):
        if arg in {"mult", "add", "sub", "inv", "lift"}:
            d_end = i
            break
    else:
        d_end = len(sys.argv)
    d = [int(x) for x in sys.argv[3:d_end]]
    operations = set(sys.argv[d_end:])

    GR = mkring(k, d[0])
    if len(d) > 1:
        GR2 = mkring(k, d[1])
    
    print("============================ generated GR ==============================")
    seq_permutations = all_permutations(d[0])
    print("====================== generated seq_permutations ======================")
    
    for i in range(n):
        test_lift_homorphism(GR, GR2, k, d[0], d[1])
    print("========================== test lift homomorphism ======================")
    
    path = "../../TestVectors/"
    if 'mult' in operations:
        with open(path + "GRMultiplication.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testmul(GR)) + '\n')
    if 'add' in operations:
        with open(path + "GRAddition.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testadd(GR)) + '\n')
    if 'sub' in operations:
        with open(path + "GRSubtraction.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testsub(GR)) + '\n')
    if 'inv' in operations:
        with open(path + "GRInverse.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testinv(GR, k, d[0], seq_permutations)) + '\n')
    if 'lift' in operations:
        with open(path + "GRLift.txt", "w") as f:
            for _ in range(n):
                f.write(" ; ".join(testlift(GR, GR2, k, d[0], d[1])) + '\n')
    
    
    print("===================== generated test vectors =============================")
