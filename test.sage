# F1 = GF(2^2, 'α', modulus="minimal_weight")
# F2 = GF(2^4, 'β', modulus="minimal_weight")


# print(F2.modulus())
# print(F1.modulus().isprimitive())
# print(F2.modulus().isprimitive())
# print("GF roots:",F2['x'](F1.modulus()).roots()[0][0])


# Z2 = IntegerModRing(2)

# Z22 = QuotientRing(Z2['x'], x^2+ x+ 1)

# Z24 = QuotientRing(Z2['x'], x^4 +x^3+x^2+x+1)

# image = Z24['x'](Z22.modulus()).roots(multiplicities=False)

# print(image)


# test
F1 = GF(2^2, 'β', modulus="minimal_weight")
F2 = GF(2^4, 'α', modulus="minimal_weight")
# F3 = GF(2^8, 'c', modulus="minimal_weight")
# 
# aa = F3['x'](F1.modulus()).roots()[0][0]
# res = set()
# embeddings = []
# for i in range(4):
#     embeddings.append(aa^i)
# 
# for b in range(2^4):
#     tmp = F3(0);
#     for i in range(4):
#         if b & (1 << i):
#             tmp += embeddings[i]
#     res.add(tmp)
# 
print(F1.modulus())
print(F2.modulus())
print(F1.modulus().is_primitive())
print(F2.modulus().is_primitive())
print("GF roots: ", F2['x'](F1.modulus()).roots())
# print(F3['x'](F1.modulus()).roots())
# print("res =", res)
# print(len(res))

Z2 = IntegerModRing(2^4)
Z2X = PolynomialRing(Z2, 'x')
Z22 = QuotientRing(Z2X, x^2+x+1)
# Z24 = IntegerModRing(2^4)
Z24 = QuotientRing(Z2X, x^4+x+1)

roots_list = Z24['x'](x^2+x+1).roots(multiplicities=False)

print(roots_list)

root_image = roots_list[0]

int_mod = Z2.cardinality()
images = [Z24['x'](k) for k in range(int_mod)]
for i in range(1, Z22.modulus().degree()):
    pre_len = len(images)
    for j in range(1, int_mod):
        images += [(elem + Z24['x'](j) * root_image^i) for elem in images[:pre_len]]

# a * x + b (0 <= a, b <= 2^4 - 1)
print("images: ",images)
print(len(images))

for a in images:
    for b in images:
        if not ((a + b) in images and (a * b) in images):
            print("Error")
            break

# print(root_image)

print(root_image^2 + root_image + 1)
# print(res[2] + res[2])




tables = []
for k in range(2, 9):
    F = GF(2^k, 'α', modulus="minimal_weight")
    inj = F.fetch_int
    proj = lambda x: x.integer_representation()
    tables.append((k, [[proj(inj(a) * inj(b)) for b in range(2^k)] for a in range(2^k)] + [[0] + [proj(inj(a)^-1) for a in range(1, 2^k)]]))

textual_tables = []
for k, tb in tables:
    textual_tables.append("constexpr inline std::uint8_t mul%d[(1<<%d) + 1][1<<%d] = %s;" % (k, k, k, str(tb).translate("".maketrans("[]", "{}"))))

with open("gftables.h", "w") as f:
    f.write("""
// This file was automatically generated, changes may be overwritten
#pragma once
#include <cstdint>
namespace gftables {
    %s
} // namespace gftables
""" % "\n    ".join(textual_tables))



embeddings = {}
for base_k in GENERATE_EMBEDDINGS:
    F1 = GF(2^base_k, 'α', modulus="minimal_weight")
    extension_factor = 1
    while extension_factor * base_k <= 128:
        ext_k = extension_factor * base_k
        F2 = GF(2^ext_k, 'β', modulus="minimal_weight")
        el = F2['x'](F1.modulus()).roots()[0][0]
        embeddings[(base_k, ext_k)] = [(el^i).integer_representation() for i in range(base_k)]
        extension_factor += 1

