import sys

## test Z2K addition
def testadd(d):
    F1 = GF(2^d, name="z", modulus="minimal_weight", repr='int')
    a = F1.random_element()
    b = F1.random_element()
    return [str(a), str(b), str(a + b)]


## test Z2K addition
def testmul(d):
    F1 = GF(2^d, name="z", modulus="minimal_weight", repr='int')
    a = F1.random_element()
    b = F1.random_element()
    return [str(a), str(b), str(a * b)]

# TODO: randomize the n, d, and ops
# TODO: put the d, ops into the output file and make test recognize it
if __name__ == "__main__":
    n = int(sys.argv[1])
    d = int(sys.argv[2])
    muloradd = sys.argv[3] == "multiply"
    for _ in range(n):
        sys.stdout.write(" ".join(testmul(d)) + '\n' if muloradd else " ".join(testadd(d)) + '\n')
    #sys.stdout.write('Test ended'+ '\n')
