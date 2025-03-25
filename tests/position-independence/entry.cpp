#include "position-independence.hpp"
#include <al/al.hpp>
#include <windows.h>

AL_ENTRY int entry() {
    auto kernel32_by_name  = GM(L"KERNEL32.DLL");
    if (!kernel32_by_name) {
        return EXIT_FAILURE;
    }

    auto kernel32_by_stack_string  = GM(L"KERNEL32.DLL", stack);
    if (kernel32_by_stack_string != kernel32_by_name) {
        return EXIT_FAILURE;
    }

    auto kernel32_by_ror13 = GM(L"KERNEL32.DLL", ror13);
    if (kernel32_by_ror13 != kernel32_by_stack_string) {
        return EXIT_FAILURE;
    }

    auto kernel32_by_djb2  = GM(L"KERNEL32.DLL", djb2);
    if (kernel32_by_djb2 != kernel32_by_ror13) {
        return EXIT_FAILURE;
    }

    auto kernel32_by_xor   = GM(L"KERNEL32.DLL", xor);
    if (kernel32_by_xor != kernel32_by_djb2) {
        return EXIT_FAILURE;
    }

    auto GetStdHandle_by_name  = GP(kernel32_by_name, GetStdHandle);
    if (!GetStdHandle_by_name) {
        return EXIT_FAILURE;
    }

    auto GetStdHandle_by_stack_string  = GP(kernel32_by_name, GetStdHandle, stack);
    if (GetStdHandle_by_stack_string != GetStdHandle_by_name) {
        return EXIT_FAILURE;
    }

    auto GetStdHandle_by_ror13 = GP(kernel32_by_name, GetStdHandle, ror13);
    if (GetStdHandle_by_ror13 != GetStdHandle_by_stack_string) {
        return EXIT_FAILURE;
    }

    auto GetStdHandle_by_djb2  = GP(kernel32_by_name, GetStdHandle, djb2);
    if (GetStdHandle_by_djb2 != GetStdHandle_by_ror13) {
        return EXIT_FAILURE;
    }

    auto GetStdHandle_by_xor   = GP(kernel32_by_name, GetStdHandle, xor);
    if (GetStdHandle_by_xor != GetStdHandle_by_djb2) {
        return EXIT_FAILURE;
    }

    auto WriteConsoleW_ = GP(kernel32_by_name, WriteConsoleW);
    if (!WriteConsoleW_) {
        return EXIT_FAILURE;
    }

    auto WriteConsoleA_ = GP(kernel32_by_name, WriteConsoleA);
    if (!WriteConsoleA_) {
        return EXIT_FAILURE;
    }

    const char* four_chars = "444"_stack;
    const char* eight_chars = "8888888"_stack;
    const char* sixteen_chars = "16161616161616"_stack;
    const char* thirty_two_chars = "3232323232323232323232323323232"_stack;

    WriteConsoleA_(
        GetStdHandle_by_name(STD_OUTPUT_HANDLE),
        four_chars,
        strlen(four_chars), // strlen is provided by al::intrinsic
        nullptr,
        nullptr
    );

    WriteConsoleA_(
        GetStdHandle_by_name(STD_OUTPUT_HANDLE),
        eight_chars,
        strlen(eight_chars),
        nullptr,
        nullptr
    );

    WriteConsoleA_(
        GetStdHandle_by_name(STD_OUTPUT_HANDLE),
        sixteen_chars,
        strlen(sixteen_chars),
        nullptr,
        nullptr
    );

    WriteConsoleA_(
        GetStdHandle_by_name(STD_OUTPUT_HANDLE),
        thirty_two_chars,
        strlen(thirty_two_chars),
        nullptr,
        nullptr
    );

    const wchar_t* four_wchars = L"4"_stack;
    const wchar_t* eight_wchars = L"888"_stack;
    const wchar_t* sixteen_wchars = L"1616161"_stack;
    const wchar_t* thirty_two_wchars = L"32323232323232"_stack;

    WriteConsoleW_(
        GetStdHandle_by_name(STD_OUTPUT_HANDLE),
        four_wchars,
        wcslen(four_wchars), // wcslen is provided by al::intrinsic
        nullptr,
        nullptr
    );

    WriteConsoleW_(
        GetStdHandle_by_name(STD_OUTPUT_HANDLE),
        eight_wchars,
        wcslen(eight_wchars),
        nullptr,
        nullptr
    );

    WriteConsoleW_(
        GetStdHandle_by_name(STD_OUTPUT_HANDLE),
        sixteen_wchars,
        wcslen(sixteen_wchars),
        nullptr,
        nullptr
    );

    WriteConsoleW_(
        GetStdHandle_by_name(STD_OUTPUT_HANDLE),
        thirty_two_wchars,
        wcslen(thirty_two_wchars),
        nullptr,
        nullptr
    );
    return EXIT_SUCCESS;
}