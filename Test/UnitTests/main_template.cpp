#include <iostream>
#include <chrono>
#include "galois_ring.hpp"
#include "protocol.hpp"

#define P {{ P }}
#define R {{ R }}
#define K {{ K }}

int main() {
#if K == -1
    using Ring = GaloisRing<P, R>;
#else
    using Ring = GaloisRing<P, R, K>;
#endif
    Protocol<Ring> protocol;
    auto start = std::chrono::high_resolution_clock::now();
    bool result = protocol.run_check();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Ring: " << Ring::name() << "\nResult: " << result
              << "\nElapsed: " << duration << " us" << std::endl;
    return 0;
}
