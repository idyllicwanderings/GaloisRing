
GENERATE_EMBEDDINGS = [4]
# No base = 1
BASE_SIZE = [2]
EXT_SIZE = 4

embeddings = {}
for base_k in BASE_SIZE:
    print("Z2K",base_k)
    Z2 = IntegerModRing(2^base_k)
    Z2K = PolynomialRing(Z2, 'x')
    for d0 in GENERATE_EMBEDDINGS:
        print("d0",2^d0)
        # find the primitive polynomial in GF
        F1 = GF(2^d0, 'α', modulus="minimal_weight")
        extension_factor = 4
        while extension_factor * d0 <= EXT_SIZE:
            ext_d0 = extension_factor * d0
            print("ext",ext_d0)
            F2 = GF(2^ext_d0, 'β', modulus="minimal_weight")
            
            ext_Z2K = QuotientRing(Z2K, F1.modulus())
            lift_Z2K = QuotientRing(Z2K, F2.modulus())

            root = lift_Z2K['x'](F1.modulus()).roots(multiplicities=False)[0]
            embeddings[(base_k,(d0, ext_d0))] = [(root^i) for i in range(d0)]
            extension_factor += 1

print(embeddings)

