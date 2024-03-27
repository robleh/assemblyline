#include <al/intrinsic.hpp>

#pragma function(memset)
void* memset(void* dest, int val, size_t len) {
    auto ptr = static_cast<unsigned char*>(dest);
    while (len-- > 0)
        *ptr++ = val;
    return dest;
}