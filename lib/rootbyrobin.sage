import itertools
try:
    from tqdm import trange as range
except ImportError:
    pass
    
k = 64
P1.<z> = Zmod(2^k)["z"]
def mkring(d, **kw):
    return P1.quo(P1(GF(2^d, name="z", modulus="minimal_weight").modulus().list()), **kw)

R1.<w> = mkring(4)
print(R1)
R2.<z> = mkring(16)
print(R2)
P2.<x> = R2["x"]

def naive():
    hom = P2(R1.modulus().list()).any_root()
    return lambda x: x.lift()(x = hom)


F = GF(2)
def lift_root():
    F1 = F.extension(R1.modulus().change_ring(F), name='z')
    F2 = F.extension(R2.modulus().change_ring(F), name='z')
    root = [ZZ(x) for x in R1.modulus().change_ring(F2).any_root().list()]
    for i in range(1, k):
        R3 = Zmod(2^(i + 1))['z'].quo(R2.modulus().change_ring(Zmod(2^(i + 1))), names=("z",))
        M = R1.modulus().change_ring(R3)
        # TODO: can we do something smarter than this enumeration? :)
        for top in itertools.product([0, 1], repeat=F2.degree()):
            new_root = [x + t * 2^i for x, t in zip(root, top)]
            if (M(z = R3(new_root))) == 0:
                root = new_root
                break
        else:
            assert False, "Couldn't lift"
    hom = R2(root)
    return lambda x: x.lift()(z = hom)

φ = lift_root()

def test():
    a = R1.random_element()
    b = R1.random_element()
    assert φ(-a) == -φ(a)
    assert φ(a + b) == φ(a) + φ(b)
    assert φ(a + b) == φ(a) + φ(b)
    assert a != φ(a)
    assert φ(a).parent() is R2

for _ in range(1000):
    test()
print("Success")


