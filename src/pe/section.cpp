
namespace al::pe {

struct section_header_sentinel {};

struct section_headers {
    uintptr_t             m_base{};
    PIMAGE_SECTION_HEADER m_section_headers;
    unsigned short        m_count{ 0 };

    section_headers(HMODULE pe) : m_base{ reinterpret_cast<uintptr_t>(pe) } {
        auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(m_base);
        auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(m_base + dos->e_lfanew);
        m_n_sections = nt->FileHeader.NumberOfSections;
        m_section_header = IMAGE_FIRST_SECTION(nt);
    }

    struct iterator {
        using value_type = PIMAGE_SECTION_HEADER;
        using difference_type = std::ptrdiff_t;

        iterator() = default;

        iterator(const PIMAGE_SECTION_HEADER first, unsigned short count) :
            m_first{ first },
            m_current{ first },
            m_count{ count }
        {}

        const value_type& operator*() const {
            return *m_current;
        }

        const value_type* operator->() const {
            return m_current;
        }

        iterator& operator++() {
            ++m_current;
            return *this;
        }

        iterator operator++(int) {
            auto tmp = *this;
            ++* this;
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return m_current == other.m_current;
        }

        bool operator==(section_header_sentinel) const {
            return m_first + m_count == m_current;
        }

    private:
        PIMAGE_SECTION_HEADER m_first{};
        PIMAGE_SECTION_HEADER m_current{};
        unsigned short        m_count{ 0 };
    };

    iterator begin() const {
        return iterator{
            &m_section_headers[0],
            m_count
        };
    }

    section_header_sentinel end() const {
        return {};
    }
};

PIMAGE_SECTION_HEADER find_section_header(
    const section_headers headers,
    const std::string_view name
) {
    auto match = std::ranges::find_if(
        headers,
        [=](const PIMAGE_SECTION_HEADER h) -> bool {
            auto comparison = name.compare(static_cast<const char*>(h->Name));
            if (0 == comparison) {
                return true;
            }
        }
    );

    //ternary?
    return (match != headers.end()) ? match : nullptr;
}

std::byte* get_section(
    const HMODULE pe,
    const std::string_view name
) {
    section_headers headers{ pe };
    auto header = find_section_header(headers, name);
    if (!header) {
        return {};
    }

    return static_cast<std::byte*>(
        reinterpret_cast<uintptr_t>(pe) + header->VirtualAddress
    );
}

std::byte* get_raw_section(
    void* file
    const std::string_view name
) {
    section_headers headers{ pe };
    auto header = find_section_header(headers, name);
    if (!header) {
        return {};
    }

    return static_cast<std::byte*>(
        reinterpret_cast<uintptr_t>(pe) + header->PointerToRawData
    );
}

} // namespace al::pe