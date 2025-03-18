#include <al/intrinsic.hpp>
#include <cstdint>

extern "C" {

#pragma function(memcmp)
int memcmp(const void* lhs, const void* rhs, size_t n) {
    const unsigned char* lhs_bytes = static_cast<const unsigned char*>(lhs);
    const unsigned char* rhs_bytes = static_cast<const unsigned char*>(rhs);

    while (n-- > 0) {
        if (*lhs_bytes++ != *rhs_bytes++) {
            return lhs_bytes[-1] < rhs_bytes[-1] ? -1 : 1;
        }
    }
    return 0;
}

// https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcpy.c
#pragma function(memcpy)
void* memcpy(void* dest, const void* src, size_t len) {
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);
    while (len--)
        *d++ = *s++;
    return dest;
}

// https://opensource.apple.com/source/network_cmds/network_cmds-481.20.1/unbound/compat/memmove.c.auto.html
#pragma function(memmove)
void* memmove(void* dest, const void* src, size_t n)
{
    uint8_t* from = (uint8_t*)src;
    uint8_t* to = (uint8_t*)dest;

    if (from == to || n == 0)
        return dest;
    if (to > from && to - from < (int)n) {
        /* to overlaps with from */
        /*  <from......>         */
        /*         <to........>  */
        /* copy in reverse, to avoid overwriting from */
        int i;
        for (i = n - 1; i >= 0; i--)
            to[i] = from[i];
        return dest;
    }
    if (from > to && from - to < (int)n) {
        /* to overlaps with from */
        /*        <from......>   */
        /*  <to........>         */
        /* copy forwards, to avoid overwriting from */
        size_t i;
        for (i = 0; i < n; i++)
            to[i] = from[i];
        return dest;
    }
    memcpy(dest, src, n);
    return dest;
}

#pragma function(memset)
void* memset(void* dest, int val, size_t len) {
    auto ptr = static_cast<unsigned char*>(dest);
    while (len-- > 0)
        *ptr++ = val;
    return dest;
}

#pragma function(strlen)
size_t strlen(const char* s) {
    const char* s0 = s;

    if (s == 0) {
        return 0;
    }

    while (*s != 0) {
        s++;
    }

    return (s - s0);
}

#pragma function(wcslen)
size_t wcslen(const wchar_t* start) {
    const wchar_t* end = start;
    for (; *end != L'\0'; ++end)
        ;
    return end - start;
}

#pragma function(strcmp)
int strcmp(const char* stra, const char* strb) {
    while (*stra && *strb && *stra == *strb)
        ++stra, ++strb;

    if (*stra < *strb) return -1;
    if (*stra > *strb) return 1;
    return 0;
}

#pragma function(wcscmp)
int wcscmp(const wchar_t* lhs, const wchar_t* rhs) {
    while (*lhs && *rhs && *lhs == *rhs)
        ++lhs, ++rhs;

    if (*lhs < *rhs) return -1;
    if (*lhs > *rhs) return 1;
    return 0;
}

} // export
