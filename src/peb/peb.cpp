#include <al/peb.hpp>

namespace al::peb {

memory_order_links::memory_order_links(PPEB peb) noexcept : m_peb{ peb } {}

memory_order_links::Iterator memory_order_links::begin() noexcept {
    return Iterator{ m_peb->Ldr->InMemoryOrderModuleList.Flink };
}

memory_order_links::Iterator memory_order_links::end() noexcept {
    return Iterator{ &m_peb->Ldr->InMemoryOrderModuleList };
}

// Move these
memory_order_links::Iterator::Iterator() noexcept = default;
memory_order_links::Iterator::Iterator(LIST_ENTRY* entry) noexcept : m_entry{ entry } {}

memory_order_links::Iterator::value_type& memory_order_links::Iterator::operator*() const noexcept {
    return *reinterpret_cast<value_type*>(CONTAINING_RECORD(
        m_entry,
        LDR_DATA_TABLE_ENTRY,
        InMemoryOrderLinks
    ));
}

memory_order_links::Iterator::value_type* memory_order_links::Iterator::operator->() const noexcept {
    return reinterpret_cast<value_type*>(CONTAINING_RECORD(
        m_entry,
        LDR_DATA_TABLE_ENTRY,
        InMemoryOrderLinks
    ));
};

memory_order_links::Iterator& memory_order_links::Iterator::operator++() noexcept {
    m_entry = m_entry->Flink;
    return *this;
}

memory_order_links::Iterator memory_order_links::Iterator::operator++(int) noexcept {
    Iterator tmp = m_entry;
    m_entry = m_entry->Flink;
    return tmp;
}

bool operator==(const memory_order_links::Iterator& a, const memory_order_links::Iterator& b) noexcept {
    return a.m_entry == b.m_entry;
}

wchar_t* entry_to_name(const LDR_DATA_TABLE_ENTRY& entry) noexcept {
    return entry.BaseDllName.Buffer;
}

} // namespace al::detail
