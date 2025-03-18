#include "test.hpp"
#include "dll.hpp"
#include <al/al.hpp>

AL_ENTRY int entry() {
    log_success(L"exe pic");
    return 0;
}