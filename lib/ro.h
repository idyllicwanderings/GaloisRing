// Random Oracle

// give an input, pass to the hash function, then to the PRG , then store a pointer to PRG, 
// then return the truncation of a PRG output

#ifndef RANDOM_H
#define RANDOM_H

#include "SimpleFIPS202.h"
#include "KeccakPRG.h"

#define SEED_BYTE_LEN 16
#define PRG_BLK_LEN 254
#define SHA3_WIDTH 256

class Random {
    KeccakWidth1600_SpongePRG_Instance PRG_instance_;
    uint8_t seed_[SEED_BYTE_LEN];
    uint8_t hash_bytes_[SHA3_WIDTH / 8];

   public:
    Random() { KeccakWidth1600_SpongePRG_Initialize(&PRG_instance_, PRG_BLK_LEN); }

    void random_bytes(uint8_t *out, unsigned int outByteLen) {
        seed(seed_, SEED_BYTE_LEN);
        hash(seed_, hash_bytes_, SHA3_WIDTH);
        PRG(hash_bytes_, SHA3_WIDTH, out, outByteLen);
    }
  
    void random_bits() {
        // TODO
    }
   
   private:
    void PRG(const uint8_t *input, unsigned int inputByteLen, uint8_t *out, unsigned int outByteLen) {
        KeccakWidth1600_SpongePRG_Feed(PRG_instance_, input, inputByteLen);
        KeccakWidth1600_SpongePRG_Fetch(PRG_instance_, out, outByteLen);
    }
   
    void hash(const uint8_t *input, unsigned int inputByteLen, uint8_t *out) {
        int result = SHA3_#SHA3_WIDTH(out, input, inputByteLen); 
        assert(result == 0);
    }

    void seed(uint8_t *out, unsigned int outByteLen) {
        std::ifstream urandom_stream("/dev/u/random", std::ios::in | std::ios::binary);
        if (!urandom_stream) std::cout << "cannot open dev/u/random " << std::endl;
        urandom_stream.read(reinterpret_cast<char*>(out), outByteLen);
        if (!urandom_stream) std::cout << "error reading dev/u/random" << std::endl;
    }

};

#endif