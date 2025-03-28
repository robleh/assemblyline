#include <gtest/gtest.h>
#include <al/al.hpp>

enum module_ror13 {
    KERNEL32 = 0x6E2BCA17,
    USER32   = 0xF210A0D8,
};

enum proc_ror13{
    LOADLIBRARYW = 0x8A8B468C,
    MESSAGEBOXW  = 0x9ACA96AE,
};

TEST(HashTest, Ror13Ansi) {
    EXPECT_EQ(
        proc_ror13::LOADLIBRARYW,
        al::ror13("LoadLibraryW")
    ) << "Calculated incorrect ror13 hash of ANSI string 'LoadLibraryW'";

    EXPECT_EQ(
        proc_ror13::LOADLIBRARYW,
        static_cast<unsigned long>("LoadLibraryW"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'LoadLibraryW'";

    EXPECT_EQ(
        proc_ror13::MESSAGEBOXW,
        al::ror13("MessageBoxW")
    ) << "Calculated incorrect ror13 hash of ANSI string 'MessageBoxW'";

    EXPECT_EQ(
        proc_ror13::MESSAGEBOXW,
        static_cast<unsigned long>("MessageBoxW"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'MessageBoxW'";
}

TEST(HashTest, Ror13Utf16) {
    EXPECT_EQ(
        module_ror13::KERNEL32,
        al::ror13("KERNEL32.DLL")
    ) << "Calculated incorrect ror13 hash of UTF-16 string 'KERNEL32.DLL'";

    EXPECT_EQ(
        module_ror13::KERNEL32,
        static_cast<unsigned long>("KERNEL32.DLL"_ror13)
    ) << "Calculated incorrect ror13 hash of UTF-16 string 'KERNEL32.DLL'";
}
