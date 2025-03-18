// hash.hpp contains constexpr hashing functions for use at compile-time and
// run-time. 
module;
#include <concepts>
#include <cstddef>
export module al:hash;

// Label sections and comments
export namespace al {

template<typename H, typename Ch>
concept Hasher = requires(H hasher, const Ch* str) {
    { hasher(str) } -> std::same_as<unsigned long>;
};

constexpr unsigned long ror13(const auto* str) {
    unsigned long hash = 0;
    for (size_t i = 0; str[i] != 0; ++i) {
        auto c = (str[i] >= 97 && str[i] <= 122) ? str[i] - 32 : str[i];
        hash = ((hash >> 13) | (hash << 19) + c);
    }
    return hash;
}

constexpr unsigned long djb2(const auto* str) {
    unsigned long hash = 5381;
    for(size_t i = 0; str[i] != 0; ++i) {
        auto c = (str[i] >= 65 && str[i] <= 90) ? str[i] + 32 : str[i];
        hash += ((hash << 5) + hash) + c;
    }
    return hash;
}
static_assert(Hasher<decltype(ror13<char>), char>);
static_assert(Hasher<decltype(ror13<wchar_t>), wchar_t>);

template<typename H, typename Ch>
concept Hash = requires(H h, H::alg_t alg, const Ch* str) {
    { alg(str) } -> std::same_as<unsigned long>;
    { h == str } -> std::same_as<bool>;
};

template<typename Ch, Hasher<Ch> auto Alg>
class hash {
    unsigned long m_hash = 0;

public:
    using alg_t = decltype(Alg);

    consteval hash(const Ch* str) {
        m_hash = Alg(str);
    }

    constexpr operator unsigned long() const {
        return m_hash;
    }

    constexpr alg_t alg() const {
        return Alg;
    }

    bool operator==(const Ch* str) const {
        return m_hash == Alg(str);
    }
};
static_assert(Hash<al::hash<char, ror13<char>>, char>);
static_assert(Hash<al::hash<wchar_t, ror13<wchar_t>>, wchar_t>);

extern "C" {

unsigned long ror13_ansi(const char* str) { return ror13(str); }
unsigned long ror13_utf16(const wchar_t* str) { return ror13(str); }
unsigned long djb2_ansi(const char* str) { return djb2(str); }
unsigned long djb2_utf16(const wchar_t* str) { return djb2(str); }

} // extern "C"
} // namespace al

export {

consteval auto operator""_ror13(const char* str, size_t) {
    return al::hash<char, al::ror13<char>>(str);
}

consteval auto operator""_ror13(const wchar_t* str, size_t) {
    return al::hash<wchar_t, al::ror13<wchar_t>>(str);
}

consteval auto operator""_djb2(const char* str, size_t) {
    return al::hash<char, al::djb2<char>>(str);
}

consteval auto operator""_djb2(const wchar_t* str, size_t) {
    return al::hash<wchar_t, al::djb2<wchar_t>>(str);
}

} // export
