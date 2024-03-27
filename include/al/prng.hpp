// prng.hpp contains compile-time pseudo randomness generation. Heavily based
// on https://gist.github.com/EvanMcBroom/ad683e394f84b623da63c2b95f6fb547.
#pragma once
#include <stdint.h>

#ifndef AL_PRNG_SEED
#define AL_PRNG_SEED 0
#endif

#define AL_PRNG_MODULUS 0x7fffffff

namespace al {

template<typename Char, size_t N>
constexpr uint32_t seed(const Char(&entropy)[N], uint32_t iv = 0) noexcept {
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

constexpr uint32_t prng(uint32_t input) noexcept {
    return (input * 48271) % AL_PRNG_MODULUS;
}

} // namespace al
