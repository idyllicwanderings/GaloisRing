Rb = Zmod(4)
Rl = Rb.extension(x^4 + x + 1)
f = Rl['y']([1, 1, 1])
F = GF(2).extension(Rl.modulus(), name='x')
r1 = F['y']([F(x.lift()) for x in f.list()]).any_root()  
#lift是把quotient ring的元素转换为Zmod再放到field上

PRb = PolynomialRing(Rb, 'z', Rl.degree())
PRl = PRb.extension(Rl.modulus())

r2_symbolic = PRl([PRb.gens()[i] * 2 + Rl(r1)[i] for i in range(Rl.degree())])

should_be_zero = f.change_ring(PRb)(r2_symbolic)
M = Matrix(Zmod(2), [[ZZ(c.coefficient({g:1})) // 2 for g in PRb.gens()] for c in should_be_zero.list()])
b = vector(Zmod(2), [ZZ(c.constant_coefficient())//2 for c in should_be_zero.list()])
z_assignment = M.solve_right(b)
r2 = Rl([x.subs({g:ZZ(a) for g, a in zip(PRb.gens(), z_assignment)}) for x in r2_symbolic.list()])
assert f(r2) == 0

# TODO: iterate and generalize to higher powers of 2
