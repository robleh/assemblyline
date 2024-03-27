#include "al/import.hpp"
#include <algorithm>

namespace al {

HMODULE get_module_handle(std::wstring_view name) noexcept {
    peb::memory_order_links modules{ 
        NtCurrentTeb()-> ProcessEnvironmentBlock
    };

    auto found = std::ranges::find_if(
        modules,
        al::by_name(name),
        peb::entry_to_name
    );
    return (modules.end() == found) ? nullptr :
        static_cast<HMODULE>(found->DllBase);
}

FARPROC get_proc_address(HMODULE mod, unsigned short ordinal) noexcept {
    pe::export_directory exports{ mod };
    pe::exported_function result = exports[ordinal];
    return (!result.address) ? nullptr : result.address;
}

FARPROC get_proc_address(HMODULE mod, std::string_view name) noexcept {
    pe::export_directory exports{ mod };

    auto compare = [name](const char* e, const char*) {
        return name.compare(e) > 0;
        };

    auto result = std::ranges::lower_bound(
        exports,
        name.data(),
        compare,
        &pe::exported_function::name
    );

    return (0 != name.compare(result->name)) ? nullptr :
        result->address;
}

} // namespace al

