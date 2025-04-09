module;
#include <span>
#include <vector>
#include <filesystem>
#include <fstream>
#include <windows.h>
export module run;

namespace al::tools {

std::vector<std::byte> read(const std::filesystem::path& path) {
    std::basic_ifstream<std::byte> file{ path, std::ios::binary };

    // ~basic_ifstream() closes the file handle
    return std::vector<std::byte>{
        std::istreambuf_iterator<std::byte>{file},
            std::istreambuf_iterator<std::byte>()
    };
}

void* allocate(std::span<std::byte> pic, unsigned long permissions) {
    // Double check flags for VirtualAlloc
    void* buf = VirtualAlloc(
        nullptr,
        pic.size_bytes(),
        MEM_COMMIT,
        permissions
    );

    std::copy_n(
        pic.data(),
        pic.size_bytes(),
        reinterpret_cast<std::byte*>(buf)
    );
    return buf;
}

export template<typename T>
class run {
    T m_pic;
    std::vector<std::byte> m_bytes;
    unsigned long m_permissions;

    // Double check flags for VirtualFree
    void unload() const {
        if (m_pic) {
            VirtualFree(static_cast<void*>(m_pic), m_bytes.size(), MEM_FREE);
        }
    }

public:
    run(
        const std::filesystem::path& path,
        unsigned long permissions = PAGE_EXECUTE_READWRITE
    ) : m_permissions(permissions) {
        m_bytes = read(path);
        m_pic = reinterpret_cast<T>(allocate(m_bytes, m_permissions));
    }

    auto operator()(auto&&... args) {
        return m_pic(std::forward<decltype(args)>(args)...);
    }

    void reload() {
        unload();
        m_pic = reinterpret_cast<T>(allocate(m_bytes, m_permissions));
    }

    ~run() {
        unload();
    }
};

} // namespace al
