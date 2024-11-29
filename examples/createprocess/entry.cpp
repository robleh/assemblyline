#include "createprocess.hpp"
#include <al/al.hpp>

using namespace al;

unsigned int entry(wchar_t* cmd) {
    auto kernel32 = GM(L"KERNEL32.DLL", by_ror13);
    if (!kernel32) {
        return EXIT_FAILURE;
    }

    auto cpw = GP(kernel32, CreateProcessW, by_djb2);
    if (!cpw) {
        return EXIT_FAILURE;
    }

    STARTUPINFOW startup{};
    PROCESS_INFORMATION info{};
    return cpw(
        nullptr,
        cmd,
        nullptr,
        nullptr,
        false,
        0,
        nullptr,
        nullptr,
        &startup,
        &info
    );
}