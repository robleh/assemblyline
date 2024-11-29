// Assembly Line library for Position Independent Code
// compilation on Windows.
#pragma once
#include "import.hpp"
#include "intrinsic.hpp"

// MSVC prepends .rdata to .text when merging them. .jmp is a placeholder for
// the jmp instruction that the al-tool patches in before dumping the PIC. It
// appears that MSVC places 8 bytes of padding at the beginning of .rdata. So,
// this section may not always be required. However, this should guarantee the
// padding is there.
#if defined(_MSC_VER) && !defined(__clang__)
#pragma section(".jmp", read)
__declspec(allocate(".jmp")) long jmp {};
#endif

// x64 stack alignment ASM
extern "C" void align_stack();

// Optional entrypoint that aligns the stack on x64. Expects a function named
// entry which it passes execution to. Requires the default AL_ORDER_FILE.
extern "C" void align() {
    align_stack();
}
