// pe.hpp contains PE iterators
#pragma once
#include <cstddef>
#include <phnt.h>

namespace al::pe {

struct exported_function {
    unsigned short ordinal{ 0 };
    FARPROC address{ nullptr };
    const char* name{ nullptr };
    const char* forward{ nullptr };
};

struct export_sentinel {};

struct export_directory {
    export_directory() = default;
    export_directory(HMODULE pe) noexcept ;

    exported_function operator[](unsigned short ordinal) noexcept;

    struct iterator {
        using value_type = exported_function;
        using difference_type = std::ptrdiff_t;

        iterator() noexcept;
        iterator(const export_directory* dir, unsigned long i) noexcept;

        const value_type& operator*() const noexcept;
        const value_type* operator->() const noexcept;
        iterator& operator++() noexcept;
        iterator operator++(int) noexcept;
        bool operator==(const iterator& other) const noexcept;
        bool operator==(export_sentinel) const noexcept;

    private:
        const export_directory*   m_dir{ nullptr };
        unsigned long             m_index{ 0 };
        mutable exported_function m_current{};
    };

    friend iterator;

    iterator begin() const noexcept;
    export_sentinel end() const noexcept;

private:
    PIMAGE_EXPORT_DIRECTORY m_dir{ nullptr };
    uintptr_t       m_base{};
    unsigned short  m_ordinal_base{};
    unsigned long   m_n_names{};
    unsigned long   m_n_functions{};
    unsigned long*  m_names{};
    unsigned long*  m_addresses{};
    unsigned short* m_ordinals{};
    unsigned long   m_size{};

    exported_function resolve(unsigned short i) const noexcept;
    const char* is_forward(uintptr_t address) const noexcept;
};

} //namespace
