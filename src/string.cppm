// string.hpp contains position independent string containers.
module;
#include <algorithm>
export module al:string;

import :prng;

// Remove large c++ types
// Condense structs
// Try to use a non template consteval object
// Revisit consteval with destructor
// How many bytes is std::copy_n vs memcmpy?
export namespace al {

template<typename Ch, size_t N>
struct buffer {
    Ch m_buf[N];

    consteval buffer(const Ch(&str)[N]) {
        std::copy_n(str, N, m_buf);
    }

    constexpr operator Ch*() const {
        return m_buf;
    }

    constexpr operator const Ch*() const {
        return m_buf;
    }

    constexpr void wipe() {
#if defined(_MSC_VER) && !defined(__clang__)
        ::RtlSecureZeroMemory(&m_buf, N);
#else
        // RtlSecureZeroMemory(&m_buf, N);
#endif
    }
};

template<typename Ch, size_t N>
struct xor_decoder {
    Ch m_buf[N];
    unsigned long m_seed{};

    constexpr xor_decoder(const Ch(&buf)[N], unsigned long seed) : m_seed{ seed } {
        std::copy_n(buf, N, m_buf);
    }

    // xor is a reserved keyword, hence the trailing underscore.
    void xor_() {
        auto stream = m_seed;

        for (auto& c : m_buf) {
            c ^= stream;
            stream = al::prng(stream);
        }
    }
    
    operator Ch*() {
        xor_();
        return m_buf;
    }

    operator const Ch*() {
        xor_();
        return m_buf;
    }

    void wipe() {
#if defined(_MSC_VER) && !defined(__clang__)
        ::RtlSecureZeroMemory(&m_buf, N);
#else
        // RtlSecureZeroMemory(&m_buf, N);
#endif
    }

    ~xor_decoder() {
        wipe();
        m_seed = 0;
    }
};

template<typename Ch, size_t N>
struct xor_buffer {
    Ch       m_buf[N];
    unsigned long m_seed;

    consteval xor_buffer(const Ch(&str)[N]) {
        m_seed = al::seed(str);
        std::copy_n(str, N, m_buf);
        xor_();
    }

    // xor is a reserved keyword, hence the trailing underscore.
    consteval void xor_() {
        auto stream = m_seed;

        for (auto& c : m_buf) {
            c ^= stream;
            stream = prng(stream);
        }
    }

    constexpr auto decoder() const {
        return xor_decoder<Ch, N>{ m_buf, m_seed };
    }
};

} //namespace al

export template<al::buffer Buf>
consteval auto operator""_stack() {
    return Buf;
}

export template<al::xor_buffer Xor>
constexpr auto operator""_xor() {
    return Xor.decoder();
}
