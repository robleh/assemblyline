#include <al/intrinsic.hpp>

#pragma function(wcslen)
size_t wcslen(const wchar_t* start) {
    const wchar_t* end = start;
    for (; *end != L'\0'; ++end)
        ;
    return end - start;
}
