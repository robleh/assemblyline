// peb.hpp contains iterators for the PEB.
#pragma once
#include <cstddef>
#include <phnt.h>

namespace al::peb {

class memory_order_links {
    PPEB m_peb;

public:
    memory_order_links(PPEB peb) noexcept;

    struct Iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = LDR_DATA_TABLE_ENTRY;

        Iterator() noexcept;
        Iterator(LIST_ENTRY* entry) noexcept;

        value_type& operator*() const noexcept;
        value_type* operator->() const noexcept;
        Iterator& operator++() noexcept;
        Iterator operator++(int) noexcept;
        friend bool operator==(const Iterator& a, const Iterator& b) noexcept;

    private:
        LIST_ENTRY* m_entry{ nullptr };
    }; 

    Iterator begin() noexcept;
    Iterator end() noexcept;
};

wchar_t* entry_to_name(const LDR_DATA_TABLE_ENTRY& entry) noexcept;

} // namespace al::detail
