#pragma once

struct strings {
    const char* four{};
    const char* eight{};
    const char* sixteen{};
    const char* thirty_two{};
    const wchar_t* wfour{};
    const wchar_t* weight{};
    const wchar_t* wsixteen{};
    const wchar_t* wthirty_two{};
};

struct results {
    strings stack{};
    strings literal{};
};

extern "C" results* entry();
using strings_t = decltype(&entry);
