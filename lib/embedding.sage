EMBEDDINGS = {}
BASE_K = [i for i in range(1,64)]
LIFT_D0 = [i for i in range(1,32)]
LIFT_D1 = 32


#TODO: delete linear time solution。。
def lift2k(k, f_list, Rl_moduli, r1, algorithm = 'log'):  #k, the Rsmall's polynomial, Rlarge's reduction polynomial, the previous roots
    Rbase = Zmod(2^k)
    Rlarge = Rbase.extension(Rl_moduli)
    f = Rlarge['ζ'](f_list) # lift是把quotient ring的元素转换为Zmod再放到field上
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
    def modulus(k):
        return GF(2^k, 'ζ', modulus="minimal_weight").modulus()
    def first_root(d_small, d_large):
        Rbase = Zmod(2)
        Rlarge = Rbase.extension(modulus(d_large))
        f = Rlarge['ζ'](modulus(d_small).list()) 
        F = GF(2).extension(Rlarge.modulus(), name='x')
        r1 = F['ζ']([F(x.lift()) for x in f.list()]).roots()[0][0]  #TODO: use any_root()
        return r1
    msmall, mlarge = modulus(d_small), modulus(d_large)
    r2 = lift2k(2, msmall, mlarge, first_root(d_small, d_large))
    if algorithm == 'linear':
        for k in range(3, k0 + 1):
            r2 = lift2k(k, msmall, mlarge, r2, algorithm = 'linear')
        r2 = r2.list()
    else:
        k = 2
        while k <= k0:
            k *= 2
            r2 = lift2k(k, msmall, mlarge, r2)
            #print("r" + str(k) + " = ", r2)
        r2 = [Zmod(1<<k0)(x) for x in r2.list()]  #truncate
    return r2
            
def genlifttables():
    for k0 in BASE_K:
        for d_small in LIFT_D0:
            extension_factor = 2
            while d_small * extension_factor < LIFT_D1:
                d_large = d_small * extension_factor
                EMBEDDINGS[(k0, d_small, d_large)] = lift(k0, d_small, d_large)
                extension_factor += 1

genlifttables()
print(EMBEDDINGS)

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

#test()
