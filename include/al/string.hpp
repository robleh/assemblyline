// string.hpp contains position independent string containers.
#pragma once 
#include "prng.hpp"
#include <algorithm>
#include <string_view>

namespace al {

template<typename Ch, size_t N>
struct buffer {
    Ch m_buf[N];

    consteval buffer(const Ch(&str)[N]) {
        std::copy_n(str, N, m_buf);
    }

    constexpr operator Ch*() const noexcept {
        return m_buf;
    }

    constexpr operator const Ch*() const noexcept {
        return m_buf;
    }

    constexpr operator const std::string_view() const noexcept
        requires std::same_as<Ch, char>
    {
        return std::string_view{ m_buf };
    }

    constexpr operator const std::wstring_view() const noexcept
        requires std::same_as<Ch, wchar_t>
    {
        return std::wstring_view{ m_buf };
    }

    constexpr void wipe() noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
        ::RtlSecureZeroMemory(&m_buf, N);
#else
        RtlSecureZeroMemory(&m_buf, N);
#endif
    }
};

template<typename Ch, size_t N>
struct xor_decoder {
    Ch m_buf[N];
    uint32_t m_seed{};

    constexpr xor_decoder(const Ch(&buf)[N], uint32_t seed) noexcept : m_seed{ seed } {
        std::copy_n(buf, N, m_buf);
    }

    // xor is a reserved keyword, hence the trailing underscore.
    void xor_() noexcept {
        auto stream = m_seed;

        for (auto& c : m_buf) {
            c ^= stream;
            stream = al::prng(stream);
        }
    }
    
    operator Ch*() noexcept {
        xor_();
        return m_buf;
    }

    operator const Ch*() noexcept {
        xor_();
        return m_buf;
    }

    operator const std::string_view() noexcept 
        requires std::same_as<Ch, char>
    {
        xor_();
        return std::string_view{ m_buf };
    }

    operator const std::wstring_view() noexcept 
        requires std::same_as<Ch, wchar_t>
    {
        xor_();
        return std::wstring_view{ m_buf };
    }

    void wipe() noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
        ::RtlSecureZeroMemory(&m_buf, N);
#else
        RtlSecureZeroMemory(&m_buf, N);
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
    uint32_t m_seed;

    consteval xor_buffer(const Ch(&str)[N]) {
        m_seed = al::seed(str, AL_PRNG_SEED);
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

    constexpr auto decoder() const noexcept {
        return xor_decoder<Ch, N>{ m_buf, m_seed };
    }
};

} //namespace al

template<al::buffer Buf>
consteval auto operator""_stack() noexcept {
    return Buf;
}

template<al::xor_buffer Xor>
constexpr auto operator""_xor() noexcept {
    return Xor.decoder();
}
