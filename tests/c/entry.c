// c-compat tests whether C programs can link to al.lib using the core interface
#include <al/al.hpp>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define KERNEL32_ROR13     0xdff7b9fd
#define LOADLIBRARYW_ROR13 0xc7ff3df7
#define MESSAGEBOXW_ROR13  0xd7c67bf7

typedef void* (LoadLibraryW_t)(const wchar_t*);
typedef unsigned long (MessageBoxW_t)(void*, const wchar_t*, const wchar_t*, unsigned long);

int entry() {
    void* kernel32 = get_module_hash(KERNEL32_ROR13, ror13_utf16);
    if (!kernel32) {
        return EXIT_FAILURE;
    }

    LoadLibraryW_t* LoadLibraryW_ = (LoadLibraryW_t*)get_proc_hash(
        kernel32,
        LOADLIBRARYW_ROR13,
        ror13_ansi
    );
    if (!LoadLibraryW_) {
        return EXIT_FAILURE;
    }
    
    void* user32 = LoadLibraryW_(L"user32.dll");
    if (!user32) {
        return EXIT_FAILURE;
    }

    MessageBoxW_t* MessageBoxW_ = (MessageBoxW_t*)get_proc_hash(
        user32,
        MESSAGEBOXW_ROR13,
        ror13_ansi
    );
    if (!MessageBoxW_) {
        return EXIT_FAILURE;
    }

    return MessageBoxW_(NULL, L"Compatible", L"C", 0);
}