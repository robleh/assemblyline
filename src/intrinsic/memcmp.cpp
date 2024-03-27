#include <al/intrinsic.hpp>

#pragma function(memcmp)
int memcmp(const void* lhs, const void* rhs, size_t n) {
    const unsigned char* lhs_bytes = (const unsigned char*)lhs;
    const unsigned char* rhs_bytes = (const unsigned char*)rhs;

    while (n-- > 0) {
        if (*lhs_bytes++ != *rhs_bytes++) {
            return lhs_bytes[-1] < rhs_bytes[-1] ? -1 : 1;
        }
    }
    return 0;
}
