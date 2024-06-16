# R_base = Z_4
Rb = Zmod(4)
# R_large = Z_4[x] / (x^4 + x + 1)
Rl = Rb.extension(x^4 + x + 1)
# f = x^2 + x + 1 \in R_base[y]
f = Rl['y']([1, 1, 1])
# F = Z_2[x] / (x^4 + x + 1)
F = GF(2).extension(Rl.modulus(), name='x')
# [r]_1
r1 = F['y']([F(x.lift()) for x in f.list()]).roots()[0][0]

# multivariate polynomial ring over R_base, with Rl.degree() variables
PRb = PolynomialRing(Rb, 'z', Rl.degree())
# univariate polynomial ring over PRb with Rl's modulus
PRl = PRb.extension(Rl.modulus())

r2_symbolic = PRl([PRb.gens()[i] * 2 + Rl(r1)[i] for i in range(Rl.degree())])
should_be_zero = f.change_ring(PRb)(r2_symbolic)
M = Matrix(Zmod(2), [[ZZ(c.coefficient({g:1})) // 2 for g in PRb.gens()] for c in should_be_zero.list()])
# M = Matrix(Zmod(4), [[ZZ(c.coefficient({g:1})) for g in PRb.gens()] for c in should_be_zero.list()])
print("r2_symbolic    =", r2_symbolic)
print("f(r2_symbolic) =", should_be_zero)
print(M)

b = vector(Zmod(2), [ZZ(c.constant_coefficient())//2 for c in should_be_zero.list()])
# b = vector(Zmod(4), [ZZ(c.constant_coefficient()) for c in should_be_zero.list()])
print(b)

z_assignment = M.solve_right(b)
r2 = Rl([x.subs({g:ZZ(a) for g, a in zip(PRb.gens(), z_assignment)}) for x in r2_symbolic.list()])
print(r2)
# assert f(r2) == 0

# TODO: iterate and generalize to higher powers of 2

Rb = Zmod(8)
Rl = Rb.extension(x^4 + x + 1)
f = Rl['y']([1, 1, 1])
print(Rl)
print(r2.parent())
r1 = r2

PRb = PolynomialRing(Rb, 'z', Rl.degree())
PRl = PRb.extension(Rl.modulus())

r2_symbolic = PRl([PRb.gens()[i] * 2 + Rl(r1.lift())[i] for i in range(Rl.degree())])
should_be_zero = f.change_ring(PRb)(r2_symbolic)
M = Matrix(Zmod(4), [[ZZ(c.coefficient({g:1})) // 2 for g in PRb.gens()] for c in should_be_zero.list()])
# M = Matrix(Zmod(8), [[ZZ(c.coefficient({g:1})) for g in PRb.gens()] for c in should_be_zero.list()])
print("r2_symbolic    =", r2_symbolic)
print("f(r2_symbolic) =", should_be_zero)
print(M)

b = vector(Zmod(4), [ZZ(c.constant_coefficient())//2 for c in should_be_zero.list()])
# b = vector(Zmod(8), [ZZ(c.constant_coefficient()) for c in should_be_zero.list()])
print(b)

z_assignment = M.solve_right(b)
print(z_assignment)

r2 = Rl([x.subs({g:ZZ(a) for g, a in zip(PRb.gens(), z_assignment)}) for x in r2_symbolic.list()])
print(r2)


Rb = Zmod(16)
Rl = Rb.extension(x^4 + x + 1)
f = Rl['y']([1, 1, 1])
r1 = r2

PRb = PolynomialRing(Rb, 'z', Rl.degree())
PRl = PRb.extension(Rl.modulus())

r2_symbolic = PRl([PRb.gens()[i] * 2 + Rl(r1.lift())[i] for i in range(Rl.degree())])
should_be_zero = f.change_ring(PRb)(r2_symbolic)
'''这一行，c.coefficient({g:1})，就是取g的系数，但是g的系数不一定是个常数，比如这里在Z_16的时候...
就不能直接转化成ZZ，需要指定其它z_i的次数
。。。。例子：c = 4*z0^2 - 4*z2^2 + 8*z1*z3 - 2*z0 + 8*z1 - 4*z2 + 4*z3
c.coefficient({z1:1}) = 8*z3 + 8 直接丢进ZZ会报错，应该按如下方式处理：：
c.coefficient({z0:0, z1:1, z2:0, z3:0}) = 8
生成dictionary的list, [{z0:1, z1:0, z2:0, z3:0}, {z0:0, z1:1, z2:0, z3:0}, {z0:0, z1:0, z2:1, z3:0}, {z0:0, z1:0, z2:0, z3:1}]
'''
coeff = [{PRb.gens()[i]:1 if i == j else 0 for i in range(Rl.degree())} for j in range(Rl.degree())]
M = Matrix(Zmod(8), [[ZZ(c.coefficient(dic)) // 2 for dic in coeff] for c in should_be_zero.list()])
print("r2_symbolic    =", r2_symbolic)
print("f(r2_symbolic) =", should_be_zero)
print(M)

b = vector(Zmod(8), [ZZ(c.constant_coefficient())//2 for c in should_be_zero.list()])
print(b)

z_assignment = M.solve_right(b)
print(z_assignment)

r2 = Rl([x.subs({g:ZZ(a) for g, a in zip(PRb.gens(), z_assignment)}) for x in r2_symbolic.list()])
print("r2 =", r2)
assert f(r2) == 0
