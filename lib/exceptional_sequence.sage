def findirreducible(k, d):
    F1 = Zmod(2^k)
    moduli = GF(2^d, name="z", modulus="minimal_weight", repr='int').modulus()
    R1.<β> = F1[]
    F2 = R1.quotient(moduli)
    while true:
        r1 = F2.random_element()
        if r1.is_unit():
            return r1

print(findirreducible(2,3))