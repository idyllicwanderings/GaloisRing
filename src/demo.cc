#include "../lib/gring.h"
#include "../lib/innprodcheck.h"
#include "../lib/compresscheck.h"
#include "../lib/saccheck.h"
#include "../lib/random.h"

void test_int128() {
    using namespace arith;
    int128 a(1234567890123456LL, 12345678LL);
    int128 b(9876543210987654LL, 98765432LL);

    int128 sum = a + b;
    int128 prod = a * b;

    std::cout << "a.low = " << a.low() << ", a.high = " << a.high() << std::endl;
    std::cout << "b.low = " << b.low() << ", b.high = " << b.high() << std::endl;

    std::cout << "sum.low = " << sum.low() << ", sum.high = " << sum.high() << std::endl;
    std::cout << "prod.low = " << prod.low() << ", prod.high = " << static_cast<uint64_t>(prod.high()) << std::endl;
}
int main() {

    test_int128();
    


    // randomness::RO ro;
    // ro.gen_random_bytes();

    // // Usage of Galois Ring of one extension
    // GR1e<64, 8> a = GR1e<64, 8>::random_element(ro);
    // GR1e<64, 8> b = GR1e<64, 8>::random_element(ro);
    // GR1e<64, 8> c;

    // auto start = std::chrono::high_resolution_clock::now();
    // c = a * b;                                          // Multiplication
    // auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "Multiplication: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

    // start = std::chrono::high_resolution_clock::now();
    


    return 1;
}
