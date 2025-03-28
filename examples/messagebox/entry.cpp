// MessageBox PIC that prompts the user
#include "messagebox.hpp"
#include <al/al.hpp>
#include <windows.h>

AL_ENTRY unsigned int entry() {
    void* kernel32 = GM(L"KERNEL32.DLL", ror13);
    if (!kernel32) {
        return EXIT_FAILURE;
    }

    auto LoadLibraryW_ = GP(kernel32, LoadLibraryW, ror13);
    if (!LoadLibraryW_) {
        return EXIT_FAILURE;
    }

    auto user32 = LoadLibraryW_(L"user32.dll");
    if (!user32) {
        return EXIT_FAILURE;
    }

    auto MessageBoxW_ = GP(user32, MessageBoxW, ror13);
    if (!MessageBoxW_) {
        return EXIT_FAILURE;
    }
    return MessageBoxW_(nullptr, L"", L"", MB_OK);
}