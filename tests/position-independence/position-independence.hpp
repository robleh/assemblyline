#pragma once

enum error {
    SUCCESS,
    KERNEL32_BY_NAME,
    KERNEL32_BY_STACK_STRING,
    KERNEL32_BY_ROR13,
    KERNEL32_BY_DJB2,
    KERNEL32_BY_XOR
};

extern "C" int entry();