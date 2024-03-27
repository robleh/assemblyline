#include "strings.hpp"
#include <al/al.hpp>

using namespace al;

void* copy_string_to_heap(auto str, size_t len, decltype(&VirtualAlloc) va) {
    auto buf = va(nullptr, len, MEM_COMMIT, PAGE_READWRITE);
    std::memcpy(buf, str, len);
    return buf;
}

extern "C"
results* entry() {
    auto kernel32 = get_module(by_ror13(L"KERNEL32.DLL"));
    if (!kernel32) {
        return {};
    }

    auto farproc = get_proc_address(kernel32, "VirtualAlloc"_stack);
    if (!farproc) {
        return {};
    }
    auto va = reinterpret_cast<void*(*)(void*, size_t, unsigned long, unsigned long)>(farproc);

    auto r = static_cast<results*>(va(
        nullptr,
        sizeof(results),
        MEM_COMMIT,
        PAGE_READWRITE
    ));

    r->stack.four = static_cast<const char*>(copy_string_to_heap("444"_stack, 4, va));
    r->stack.eight = static_cast<const char*>(copy_string_to_heap("8888888"_stack, 8, va));
    r->stack.sixteen = static_cast<const char*>(copy_string_to_heap("161616161616161"_stack, 16, va));
    r->stack.thirty_two = static_cast<const char*>(copy_string_to_heap("3232323232323232323232323323232"_stack, 32, va));
    r->stack.wfour = static_cast<const wchar_t*>(copy_string_to_heap(L"4"_stack, 4, va));
    r->stack.weight = static_cast<const wchar_t*>(copy_string_to_heap(L"888"_stack, 8, va));
    r->stack.wsixteen = static_cast<const wchar_t*>(copy_string_to_heap(L"1616161"_stack, 16, va));
    r->stack.wthirty_two = static_cast<const wchar_t*>(copy_string_to_heap(L"32323232323232"_stack, 32, va));

    r->literal.four = static_cast<const char*>(copy_string_to_heap("444", 4, va));
    r->literal.eight = static_cast<const char*>(copy_string_to_heap("8888888", 8, va));
    r->literal.sixteen = static_cast<const char*>(copy_string_to_heap("161616161616161", 16, va));
    r->literal.thirty_two = static_cast<const char*>(copy_string_to_heap("3232323232323232323232323323232", 32, va));
    r->literal.wfour = static_cast<const wchar_t*>(copy_string_to_heap(L"4", 4, va));
    r->literal.weight = static_cast<const wchar_t*>(copy_string_to_heap(L"888", 8, va));
    r->literal.wsixteen = static_cast<const wchar_t*>(copy_string_to_heap(L"1616161", 16, va));
    r->literal.wthirty_two = static_cast<const wchar_t*>(copy_string_to_heap(L"32323232323232", 32, va));

    return r;
}