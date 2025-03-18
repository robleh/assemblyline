// pic.hpp is an Assembly Line GoogleTest extension. It requires GoogleTest
// and should not be included as part of PIC.
#pragma once
#include <gtest/gtest.h>
#include <span>
#include <vector>
#include <filesystem>
#include <fstream>
#include <Windows.h>

// search_exe()
// search_pic()
// check_exe()
// check_pic()
// check_entrypoint()
// check_permissions()
// compare_exe()

#define AL_DEFAULT_TEST_DIR 
namespace fs = std::filesystem;

inline static std::vector<std::byte> read(const std::filesystem::path& path) {
    fs::path pic_path{};

    if (fs::exists(fs::current_path() / path.filename())) {
        pic_path = fs::current_path() / path.filename();
    }
    else if (std::getenv("AL_PIC_DIR")) {
        pic_path = fs::absolute(std::getenv("AL_PIC_DIR")) / path.filename();
    }
    else {
        pic_path = path;
    }
    EXPECT_TRUE(fs::exists(pic_path)) << "PIC path not found: " << pic_path;

    std::basic_ifstream<std::byte> file{ pic_path, std::ios::binary };

    // ~basic_ifstream() closes the file handle
    return std::vector<std::byte>{
        std::istreambuf_iterator<std::byte>{file},
        std::istreambuf_iterator<std::byte>()
    };
}
inline static void* allocate(std::span<std::byte> pic, unsigned long permissions) {
    // Double check flags for VirtualAlloc
    void* executable_memory = VirtualAlloc(
        nullptr,
        pic.size_bytes(),
        MEM_COMMIT,
        permissions
    );
    // ASSERT_TRUE(executable_memory);

    std::copy_n(
        pic.data(),
        pic.size_bytes(),
        reinterpret_cast<std::byte*>(executable_memory)
    );
    return executable_memory;
}

template<
    typename pic_t = decltype(&entry),
    unsigned long permissions = PAGE_EXECUTE_READWRITE
>
class PICTest : public testing::Test {
protected:
    inline static std::vector<std::byte> bytes;
    pic_t pic = nullptr;

    static void SetUpTestSuite() {
        PICTest::bytes = read(AL_PIC_PATH);
    }

    void SetUp() override {
        pic = reinterpret_cast<pic_t>(allocate(PICTest::bytes, permissions));
    }

    // Double check flags for VirtualFree
    void TearDown() override {
        VirtualFree(reinterpret_cast<void*>(pic), bytes.size(), MEM_FREE);
    }
};
