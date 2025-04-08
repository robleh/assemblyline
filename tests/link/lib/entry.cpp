#include "lib.hpp"
#include <al/al.hpp>
#include <windows.h>

void log_success(const wchar_t* msg){
    auto kernel32 = GM(L"KERNEL32.DLL");
    if (!kernel32) {
        return;
    }

    auto GetStdHandle_ = GP(kernel32, GetStdHandle);
    if (!GetStdHandle_) {
        return;
    }

    auto WriteConsoleW_ = GP(kernel32, WriteConsoleW);
    if (!WriteConsoleW_) {
        return;
    }

    WriteConsoleW_(
        GetStdHandle_(STD_OUTPUT_HANDLE),
        msg,
        wcslen(msg), // wcslen is provided by al::intrinsic
        nullptr,
        nullptr
    );
    return;
}