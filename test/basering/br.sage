import sys

def mkring(k, d):
    F1 = Zmod(2^k)
    moduli = GF(2^d, name="z", modulus="minimal_weight", repr='int').modulus()
    R1.<β> = F1[]
    F2 = R1.quotient(moduli)
    return F2

def testadd(F2):  ## test BR addition
    a = F2.random_element()
    b = F2.random_element()
    return [str(a), str(b), str(a + b)]


def testsub(F2):  ## test BR subtraction
    a = F2.random_element()
    b = F2.random_element()
    return [str(a), str(b), str(a - b)]

def testmul(F2):  ## test BR multiply
    a = F2.random_element()
    b = F2.random_element()
    return [str(a), str(b), str(a * b)]

if __name__ == "__main__":
    n = int(sys.argv[1])
    k = int(sys.argv[2])
    d = int(sys.argv[3])
    muloradd = sys.argv[4] == "multiply"   #TODO: include subtraction
    R = mkring(k,d)
    for _ in range(n):
        sys.stdout.write(" ".join(testmul(R)) + '\n' if muloradd else " ".join(testadd(R)) + '\n')
    #sys.stdout.write('Test ended'+ '\n')
