#include <gtest/gtest.h>
#include <al/al.hpp>

#define KERNEL32_ROR13     0xdff7b9fd
#define LOADLIBRARYW_ROR13 0xc7ff3df7
#define MESSAGEBOXW_ROR13  0xd7c67bf7

class HashTest : public testing::Test {
protected:
    unsigned long m_kernel32_ror13;
    unsigned long m_LoadLibraryW_ror13;
    unsigned long m_MessageBoxW_ror13;

    void SetUp() override {
        m_kernel32_ror13    = KERNEL32_ROR13;
        m_LoadLibraryW_ror13 = LOADLIBRARYW_ROR13;
        m_MessageBoxW_ror13 = MESSAGEBOXW_ROR13;
    }

    void TearDown() override {
    }
};

TEST_F(HashTest, Ror13Ansi) {
    auto test = 1;
    EXPECT_EQ(
        m_LoadLibraryW_ror13,
        al::ror13("LoadLibraryW")
    ) << "Calculated incorrect ror13 hash of ANSI string 'LoadLibraryW'";

    EXPECT_EQ(
        m_LoadLibraryW_ror13,
        static_cast<unsigned long>("LoadLibraryW"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'LoadLibraryW'";

    EXPECT_EQ(
        m_MessageBoxW_ror13,
        al::ror13("MessageBoxW")
    ) << "Calculated incorrect ror13 hash of ANSI string 'MessageBoxW'";

    EXPECT_EQ(
        m_MessageBoxW_ror13,
        static_cast<unsigned long>("MessageBoxW"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'MessageBoxW'";
}

TEST_F(HashTest, Ror13Utf16) {
    EXPECT_EQ(
        m_kernel32_ror13,
        al::ror13("KERNEL32.DLL")
    ) << "Calculated incorrect ror13 hash of UTF-16 string 'KERNEL32.DLL'";

    EXPECT_EQ(
        m_kernel32_ror13,
        static_cast<unsigned long>("KERNEL32.DLL"_ror13)
    ) << "Calculated incorrect ror13 hash of UTF-16 string 'KERNEL32.DLL'";
}
