// core-c tests whether C programs can link to al.lib using the core interface
#include <al/core.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef void* (LoadLibraryW_t)(const wchar_t*);
typedef unsigned long (MessageBoxW_t)(void*, const wchar_t*, const wchar_t*, unsigned long);

int entry() {
    void* kernel32 = get_module(L"KERNEL32.DLL");
    if (!kernel32) {
        return EXIT_FAILURE;
    }

    LoadLibraryW_t* LoadLibraryW_ = (LoadLibraryW_t*)get_proc(
        kernel32,
        "LoadLibraryW"
    );
    if (!LoadLibraryW_) {
        return EXIT_FAILURE;
    }
    
    void* user32 = LoadLibraryW_(L"user32.dll");
    if (!user32) {
        return EXIT_FAILURE;
    }

    MessageBoxW_t* MessageBoxW_ = (MessageBoxW_t*)get_proc(
        user32,
        "MessageBoxW"
    );
    if (!MessageBoxW_) {
        return EXIT_FAILURE;
    }

    return MessageBoxW_(NULL, L"Compatible", L"C", 0);
}