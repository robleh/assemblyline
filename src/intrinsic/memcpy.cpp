#include <al/intrinsic.hpp>
// https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcpy.c

#pragma function(memcpy)
void* memcpy(void* dest, const void* src, size_t len) {
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);
    while (len--)
        *d++ = *s++;
    return dest;
}
