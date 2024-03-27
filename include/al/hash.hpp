// hash.hpp contains constexpr hashing functions for use at compile-time and
// run-time. 
#pragma once
#include <string_view>
#include <numeric>

namespace al {

constexpr unsigned long ror13(std::wstring_view str) noexcept {
    return std::accumulate(
        str.cbegin(),
        str.cend(),
        0,
        [](unsigned long hash, unsigned long c) {
            c = (c > 65 && c < 90) ? c + 32 : c;
            return ((hash >> 13) | (hash << 19) + c);
        }
    );
}

constexpr unsigned long ror13(std::string_view str) noexcept {
    return std::accumulate(
        str.cbegin(),
        str.cend(),
        0,
        [](unsigned long hash, unsigned long c) {
            c = (c > 65 && c < 90) ? c + 32 : c;
            return ((hash >> 13) | (hash << 19) + c);
        }
    );
}

constexpr unsigned long djb2(std::wstring_view str) noexcept {
    return std::accumulate(
        str.cbegin(),
        str.cend(),
        5381,
        [](unsigned long hash, unsigned long c) {
            c = (c > 65 && c < 90) ? c + 32 : c;
            return ((hash << 5) + hash) + c;
        }
    );
}

constexpr unsigned long djb2(std::string_view str) noexcept {
    return std::accumulate(
        str.cbegin(),
        str.cend(),
        5381,
        [](unsigned long hash, unsigned long c) {
            c = (c > 65 && c < 90) ? c + 32 : c;
            return ((hash << 5) + hash) + c;
        }
    );
}

} // namespace al
