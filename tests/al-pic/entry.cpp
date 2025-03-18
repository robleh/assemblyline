#include <al/al.hpp>

AL_ENTRY int entry() {
    auto kernel32_by_name  = GM(L"KERNEL32.DLL");
    auto kernel32_by_ror13 = GM(L"KERNEL32.DLL"_ror13);
    auto kernel32_by_djb2  = GM(L"KERNEL32.DLL"_djb2);
    auto kernel32_by_xor   = GM(L"KERNEL32.DLL"_xor);
}