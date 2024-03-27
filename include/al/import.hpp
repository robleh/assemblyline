// import.hpp contains functions related to importing functions from shared
// libraries.
#pragma once
#include "hash.hpp"
#include "string.hpp"
#include "peb.hpp"
#include "pe.hpp"
#include <string_view>
#include <phnt.h>

// Get a module handle using custom search criteria. Module names are expected
// to be wide strings, and this macro will add the _stack suffix.
#define GM(dll, matcher) al::get_module(matcher(dll ## _stack))

// Get a pointer to an exported function using custom search criteria. The
// second parameter is a symbol not a string.
#define GP(dll, symbol, matcher) al::get_proc<decltype(symbol)*>(dll, matcher(#symbol ## _stack))

namespace al {

HMODULE get_module_handle(std::wstring_view name) noexcept;
FARPROC get_proc_address(HMODULE, std::string_view) noexcept;
FARPROC get_proc_address(HMODULE, unsigned short) noexcept;

// Predicate needs to be a unary predicate that accepts the projected type ONLY
// otherwise throw compile time error. Projection must be on LDR_DATA_TABLE_ENTRY.
HMODULE get_module(auto predicate) noexcept {
    peb::memory_order_links modules{ 
        NtCurrentTeb()-> ProcessEnvironmentBlock
    };

    auto found = std::ranges::find_if(
        modules,
        predicate, 
        peb::entry_to_name
    );
    if (modules.end() == found) {
        return nullptr;
    }

    return static_cast<HMODULE>(found->DllBase);
}

// Predicate should be constrained to accept a char* and return a bool
// Declaration is actually a pointer to declaration type so the name is unintuitive
template<typename Decl, typename Pred>
Decl get_proc(HMODULE mod, Pred predicate) noexcept {
    pe::export_directory exports{ mod };

    auto found = std::ranges::find_if(
        exports,
        predicate,
        &pe::exported_function::name
    );
    if (found == exports.end()) {
        return nullptr;
    }

    return reinterpret_cast<Decl>(found->address);
}

template<typename Decl>
Decl* cast(Decl function, FARPROC address) noexcept {
    return reinterpret_cast<Decl*>(address);
}

// Case sensitive
constexpr auto by_name(std::wstring_view name) noexcept {
    return [=](const auto& other) constexpr -> bool {
        return 0 == name.compare(other);
    };
}

// Case sensitive
constexpr auto by_name(std::string_view name) noexcept {
    return [=](const auto& other) constexpr -> bool {
        return 0 == name.compare(other);
    };
}

consteval auto by_ror13(auto name) noexcept {
    auto hash = ror13(name);
    return [=](const auto& text) constexpr -> bool {
        return hash == ror13(text);
    };
}

consteval auto by_djb2(auto name) noexcept {
    auto hash = djb2(name);
    return [=](const auto& text) constexpr -> bool {
        return hash == djb2(text);
    };
}

constexpr auto by_pdb(const std::string_view path) noexcept {
    return [=](void* entry_base) -> bool {
        auto b = reinterpret_cast<uintptr_t>(entry_base);
        auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(entry_base);
        auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(b + dos->e_lfanew);
        auto opt = &(nt->OptionalHeader);
        auto data = opt->DataDirectory;

        auto debug_rva = data[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
        auto debug_size = data[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
        if (0 == debug_rva || 0 == debug_size) {
            return false;
        }

        auto debug = reinterpret_cast<PIMAGE_DEBUG_DIRECTORY>(b + debug_rva);
        if (IMAGE_DEBUG_TYPE_CODEVIEW != debug->Type) {
            return false;
        }

        // Hardcoded PDB path offset because compiler wasn't adding base and
        // AddressOfRawData properly when casting to RSDS table struct.
        auto pdb = reinterpret_cast<const char*>(b + debug->AddressOfRawData + 0x18);

        return 0 == path.compare(pdb);
    };
}

} // namespace al
