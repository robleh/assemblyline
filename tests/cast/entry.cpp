#include "cast.hpp"
#include <al/al.hpp>

using namespace al;

extern "C"
int entry() {
    auto kernel32 = GM(L"KERNEL32.DLL", by_djb2);
    if (!kernel32) {
        return EXIT_FAILURE;
    }

    auto llw = GP(kernel32, LoadLibraryW, by_ror13);
    if (!llw) {
        return EXIT_FAILURE;
    }

    HMODULE user32 = llw(L"user32.dll"_xor);
    if (!user32) {
        return EXIT_FAILURE;
    }

    auto mba = GP(user32, MessageBoxA, by_name);
    if (!mba) {
        return EXIT_FAILURE;
    }

    // __debugbreak();
    mba(nullptr, "Casted"_xor, "Success"_stack, MB_OK);
    return EXIT_SUCCESS;
}