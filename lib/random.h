// Random Oracle

// give an input, pass to the hash function, then to the PRG , then store a pointer to PRG, 
// then return the truncation of a PRG output

#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>
#include <string>
#include <fstream>

extern "C" {
#include "SimpleFIPS202.h"
#include "KeccakPRG.h"
}

#define SEED_BYTE_LEN 16
#define SHA3_WIDTH 256
#define OUT_BYTE_LEN 256

namespace random {

class RO {

    uint8_t out_[OUT_BYTE_LEN];
    uint64_t p_;

    void gen_random_bytes() {
        // SHA3之后就是shake256
        uint8_t seed_[SEED_BYTE_LEN];
        uint8_t hash_[#SHA3_WIDTH / 8];
        seed(seed_, SEED_BYTE_LEN);
        int ret = SHA3_#SHA3_WIDTH(hash_, seed_, SEED_BYTE_LEN); 
        assert(ret == 0);
        ret = SHAKE256(out_, OUT_BYTE_LEN, hash_, #SHA3_WIDTH / 8);
        static_assert(ret == 0);
        p_ = 0;
    }

    void get_bytes(uint8_t *out, unsigned int byte_len) {
        if (p_ + byte_len <= OUT_BYTE_LEN) {
            std::memcpy(out, out_ + p_, byte_len);
            p_ += byte_len;
        } else {
        // TODO: Handle the error if requested length exceeds the available length
        }
    }

    void get_bits(uint8_t *out, unsigned int bit_len) {
        unsigned int byte_len = (bit_len + 7) / 8;
        get_bytes(out, byte_len);
        out[byte_len - 1] &= (1 << (bit_len % 8)) - 1;
    }

    void seed(uint8_t *out, unsigned int byte_len) {
        std::ifstream urandom_stream("/dev/u/random", std::ios::in | std::ios::binary);
        if (!urandom_stream) std::cout << "cannot open dev/u/random " << std::endl;
        urandom_stream.read(reinterpret_cast<char*>(out), byte_len);
        if (!urandom_stream) std::cout << "error reading dev/u/random" << std::endl;
    }

};
}

#endif