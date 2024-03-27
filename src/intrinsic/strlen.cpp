#include <al/intrinsic.hpp>

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
