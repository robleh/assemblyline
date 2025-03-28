// CreateProcess PIC which accepts a parameter
#include "createprocess.hpp"
#include <al/al.hpp>
#include <windows.h>

AL_ENTRY unsigned int entry(wchar_t* cmd) {
    auto kernel32 = GM(L"KERNEL32.DLL", ror13);
    if (!kernel32) {
        return EXIT_FAILURE;
    }

    auto CreateProcessW_ = GP(kernel32, CreateProcessW, djb2);
    if (!CreateProcessW_) {
        return EXIT_FAILURE;
    }

    STARTUPINFOW startup{};
    PROCESS_INFORMATION info{};
    wchar_t calc[] = L"calc.exe";
    return CreateProcessW_(
        nullptr,
        calc,
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