// Random Oracle

// give an input, pass to the hash function, then to the PRG , then store a pointer to PRG, 
// then return the truncation of a PRG output

#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>

extern "C" {
#include "SimpleFIPS202.h"
#include "KeccakPRG.h"
}

#define SEED_BYTE_LEN 16
#define SHA3_WIDTH 256
#define OUT_BYTE_LEN 256 * 8

namespace randomness {

// TODO: make the bits more fleixble
uint64_t to_int(const uint8_t* data, int bit_len) {
    uint64_t res = 0;
    int byte_len = (bit_len + 7) / 8; 
    byte_len = (byte_len > 8) ? 8 : byte_len;
    for (int i = 0; i < byte_len; ++i) {
        res |= static_cast<uint64_t>(data[i]) << (i * 8);
    }
    if (bit_len % 8 != 0) {
        uint64_t mask = (1ULL << bit_len) - 1;
        res &= mask;
    }
    return res;
}

class RO {
    public:
    void gen_random_bytes() {
        // SHA3之后就是shake256
        uint8_t seed_[SEED_BYTE_LEN];
        uint8_t hash_[SHA3_WIDTH / 8];
        seed(seed_, SEED_BYTE_LEN);
        //std::cout << "seed: " << to_int(seed_, 64) << std::endl;
        int ret = SHA3_256(hash_, seed_, SEED_BYTE_LEN); 
        //std::cout << "hash: " << to_int(hash_, 64) << std::endl;
        assert(ret == 0);
        ret = SHAKE256(out_, OUT_BYTE_LEN, hash_, SHA3_WIDTH / 8);
        //std::cout << "out: " << to_int(out_, 64) << std::endl;
        assert(ret == 0);
        p_ = 0;
    }

    void get_bytes(uint8_t *out, uint64_t byte_len) {
        if (p_ + byte_len <= OUT_BYTE_LEN) {
            memcpy(out, out_ + p_, byte_len);
            p_ += byte_len;
        } else {
            std::cout << "random.h error: requested length exceeds the available length" << std::endl;
        // TODO: Handle the error if requested length exceeds the available length
        }
    }

    private:
    uint8_t out_[OUT_BYTE_LEN];
    uint64_t p_;



    void get_bits(uint8_t *out, uint64_t bit_len) {
        uint64_t byte_len = (bit_len + 7) / 8;
        get_bytes(out, byte_len);
        out[byte_len - 1] &= (1 << (bit_len % 8)) - 1;
    }

    void seed(uint8_t *out, uint64_t byte_len) {
        std::ifstream urandom_stream("/dev/urandom", std::ios::in | std::ios::binary);
        if (!urandom_stream) std::cout << "cannot open dev/urandom " << std::endl;
        urandom_stream.read(reinterpret_cast<char*>(out), byte_len);
        if (!urandom_stream) std::cout << "error reading dev/urandom" << std::endl;
    }

};
}

#endif