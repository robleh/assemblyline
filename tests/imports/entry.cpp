#include "imports.hpp"
#include <al/al.hpp>

using namespace al;

extern "C"
void entry(results* r) {
    r->module_name = GM(L"ntdll.dll", by_name);
    r->module_ror13 = GM(L"ntdll.dll", by_ror13);
    r->module_djb2 = GM(L"ntdll.dll", by_djb2);

    r->import_name = static_cast<void*>(GP(
        static_cast<HMODULE>(r->module_name),
        LdrUnloadDll,
        by_name
    ));

    r->import_ror13 = static_cast<void*>(GP(
        static_cast<HMODULE>(r->module_ror13),
        LdrUnloadDll,
        by_ror13
    ));

    r->import_djb2 = static_cast<void*>(GP(
        static_cast<HMODULE>(r->module_djb2),
        LdrUnloadDll,
        by_djb2
    ));
}