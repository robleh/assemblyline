// intrinsic.hpp contains compiler intrinsic replacements.
#pragma once

extern "C" {

int memcmp(const void* lhs, const void* rhs, size_t n);
#pragma intrinsic(memcmp)

void* memcpy(void* dest, const void* src, size_t len);
#pragma intrinsic(memcpy)

void* memmove(void* dest, const void* src, size_t n);
#pragma intrinsic(memmove)

void* memset(void* dest, int val, size_t len);
#pragma intrinsic(memset)

size_t strlen(const char* s);
#pragma intrinsic(strlen)

size_t wcslen(const wchar_t* s);
#pragma intrinsic(wcslen)

} // extern "C"
