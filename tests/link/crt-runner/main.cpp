#include "test.hpp"
#include "dll.hpp"
#include <windows.h>

int wmain(int argc, wchar_t* argv[]) {
    log_success(L"crt static lib\n");
    dll_export();
    return EXIT_SUCCESS;
}