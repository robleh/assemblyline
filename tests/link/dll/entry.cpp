#include "dll.hpp"
#include "lib.hpp"
#include <al/al.hpp>

AL_ENTRY __declspec(dllexport) int dll_export() {
    log_success(L"pic dll\n");
    return 0;
}