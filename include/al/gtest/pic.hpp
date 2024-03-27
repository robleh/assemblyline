// test.hpp is an Assembly Line GoogleTest extension. It requires GoogleTest
// and should not be included as part of PIC.
#pragma once
#include <gtest/gtest.h>
#include <span>
#include <vector>
#include <filesystem>
#include <fstream>
#include <print>
#include <Windows.h>

// Add GTest asserts within your code
// Assert that file was found
// Assert that bytes read > 0
// Add log messages to Gtest output
inline std::vector<std::byte> read_file(const std::filesystem::path& path) {
    std::basic_ifstream<std::byte> file{ path, std::ios::binary };

    // ~basic_ifstream() closes the file handle
    return std::vector<std::byte>{
        std::istreambuf_iterator<std::byte>{file},
        std::istreambuf_iterator<std::byte>()
    };
}

// Add GTest asserts within your code
// Assert memory was allocated
// Assert pic_nx and executable buffer match
// Add log messages to gtest stdout
inline void* allocate_memory(std::span<std::byte> pic, unsigned long permissions) {
    // Double check flags for VirtualAlloc
    void* executable_memory = ::VirtualAlloc(
        nullptr,
        pic.size_bytes(),
        MEM_COMMIT,
        permissions
    );
    if (!executable_memory) {
        exit(EXIT_FAILURE);
    }
    std::println("Allocated buffer at {}", executable_memory);

    std::copy_n(
        pic.data(),
        pic.size_bytes(),
        reinterpret_cast<std::byte*>(executable_memory)
    );
    return executable_memory;
}

// Add Gtest asserts
template<class PIC>
class PICTest : public testing::Test {
protected:
    inline static std::vector<std::byte> pic_nx;

    static void SetUpTestSuite() {
       PIC::pic_nx = read_file(PIC::path);
    }

    void SetUp() override {
        void* buffer = allocate_memory(PIC::pic_nx, PIC::permissions);
        static_cast<PIC*>(this)->m_pic = static_cast<decltype(static_cast<PIC*>(this)->m_pic)>(buffer);
    }

    // Double check flags for VirtualFree
    void TearDown() override {
        VirtualFree(static_cast<PIC*>(this)->m_pic, PIC::pic_nx.size(), MEM_FREE);
    }
};
