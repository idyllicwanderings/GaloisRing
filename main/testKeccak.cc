#include "third_party/XKCP/SimpleFIPS202.h"
#include <cstdint>

// TODO: write a (seed+ counter) as a feed into KeccakPRG function
template <typename T>
T random(const unsigned char *seed) {
    unsigned char res[ceil(sizeof(T) / 8)];
        
    if (SHA3_256(res, seed, strlen((const char *)seed))) {
        std::cout << "failed" << std::endl;
        counter ++;
    }
    return res;

}

int main() {
    const unsigned char *seed = "i am so juliaaaa";
    random<uint32_t>(seed);
}