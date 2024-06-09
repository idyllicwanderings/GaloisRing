import sys

def testadd(d):  ## test Z2K addition
    F1 = GF(2^d, name="z", modulus="minimal_weight", repr='int')
    a = F1.random_element()
    b = F1.random_element()
    return [str(a), str(b), str(a + b)]


def testmul(d):  ## test Z2K addition
    F1 = GF(2^d, name="z", modulus="minimal_weight", repr='int')
    a = F1.random_element()
    b = F1.random_element()
    return [str(a), str(b), str(a * b)]

if __name__ == "__main__":
    n = int(sys.argv[1])
    print(n)
    d = int(sys.argv[2])
    print(d)
    muloradd = sys.argv[3] == "multiply"
    print(sys.argv[3])
    for _ in range(n):
        sys.stdout.write(" ".join(testmul(d)) + '\n' if muloradd else " ".join(testadd(d)) + '\n')
    #sys.stdout.write('Test ended'+ '\n')
