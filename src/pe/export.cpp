#include <al/pe.hpp>

namespace al::pe {

export_directory::export_directory(HMODULE pe) noexcept : m_base{ reinterpret_cast<uintptr_t>(pe) } {

    auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(m_base);
    auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(m_base + dos->e_lfanew);
    auto opt = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(&nt->OptionalHeader);
    auto data = reinterpret_cast<PIMAGE_DATA_DIRECTORY>(&opt->DataDirectory);
    auto m_dir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(m_base + data[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    m_size = data->Size;
    m_n_functions = m_dir->NumberOfFunctions;
    m_n_names = m_dir->NumberOfNames;
    m_ordinal_base = m_dir->Base;
    m_addresses = reinterpret_cast<unsigned long*>(m_base + m_dir->AddressOfFunctions);
    m_names = reinterpret_cast<unsigned long*>(m_base + m_dir->AddressOfNames);
    m_ordinals = reinterpret_cast<unsigned short*>(m_base + m_dir->AddressOfNameOrdinals);
}

exported_function export_directory::operator[](unsigned short ordinal) noexcept {
    if (ordinal < m_ordinal_base || ordinal > m_n_functions) {
        return {};
    }

    return exported_function{
        ordinal,
        reinterpret_cast<FARPROC>(m_base + m_addresses[ordinal - m_ordinal_base]),
        nullptr,
        is_forward(m_addresses[ordinal - m_ordinal_base])
    };
}

export_directory::iterator::iterator() noexcept = default;

export_directory::iterator::iterator(const export_directory* dir, unsigned long i) noexcept :
    m_dir{ dir },
    m_index{ i }
{}

const export_directory::iterator::value_type& export_directory::iterator::operator*() const noexcept {
    m_current = m_dir->resolve(m_index);
    return m_current;
}

const export_directory::iterator::value_type* export_directory::iterator::operator->() const noexcept {
    m_current = m_dir->resolve(m_index);
    return &m_current;
}

export_directory::iterator& export_directory::iterator::operator++() noexcept {
    ++m_index;
    return *this;
}

export_directory::iterator export_directory::iterator::operator++(int) noexcept {
    auto tmp = *this;
    ++*this;
    return tmp;
}

bool export_directory::iterator::operator==(const iterator& other) const noexcept {
    return m_index == other.m_index;
}

bool export_directory::iterator::operator==(export_sentinel) const noexcept {
    return m_index == m_dir->m_n_names;
}

export_directory::iterator export_directory::begin() const noexcept {
    return iterator{ this, 0 };
}

export_sentinel export_directory::end() const noexcept { return {}; }

exported_function export_directory::resolve(unsigned short i) const noexcept {
    return exported_function{
        static_cast<unsigned short>(m_ordinal_base + m_ordinals[i]),
        reinterpret_cast<FARPROC>(m_base + m_addresses[m_ordinals[i]]),
        reinterpret_cast<const char*>(m_base + m_names[i]),
        is_forward(m_addresses[m_ordinals[i]])
    };
}

const char* export_directory::is_forward(uintptr_t address) const noexcept {
    if (address < m_base || address >(m_base + m_size)) {
        return nullptr;
    }
    return reinterpret_cast<const char*>(address);
}

} // namespace al::pe