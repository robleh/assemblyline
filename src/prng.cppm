// prng.hpp contains compile-time pseudo randomness generation. Heavily based
// on https://gist.github.com/EvanMcBroom/ad683e394f84b623da63c2b95f6fb547.
module;
#include <cstddef>
export module al:prng;

#ifndef AL_PRNG_SEED
#define AL_PRNG_SEED 0
#endif

#define AL_PRNG_MODULUS 0x7fffffff

export namespace al {

template<typename Char, size_t N>
constexpr unsigned long seed(const Char(&entropy)[N], unsigned long iv = AL_PRNG_SEED) {
    for (size_t i{ 0 }; i < N; i++) {
        // Xor 1st byte of seed with input byte
        iv = (iv & ((~0) << 8)) | ((iv & 0xFF) ^ entropy[i]);
        // Rotl 1 byte
        iv = iv << 8 | iv >> ((sizeof(iv) * 8) - 8);
    }

    // The seed is required to be less than the modulus and odd
    while (iv > AL_PRNG_MODULUS) {
        iv = iv >> 1;
    }
    return iv << 1 | 1;
}

constexpr unsigned long prng(unsigned long input) {
    return (input * 48271) % AL_PRNG_MODULUS;
}

} // namespace al
