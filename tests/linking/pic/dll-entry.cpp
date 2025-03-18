#include "test.hpp"
#include <al/al.hpp>

AL_ENTRY __declspec(dllexport) int entry() {
    log_success(L"pic dll");
    return 0;
}