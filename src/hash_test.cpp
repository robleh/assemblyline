#include <gtest/gtest.h>
#include <al/al.hpp>

// Precomputed hash values of uppercase string with .dll suffix
enum utf16_ror13 {
    KERNEL32 = 0x862E96F8,
    USER32   = 0xF210A0D8,
};

// Precomputed hash values of uppercase string
enum ansi_ror13{
    LOADLIBRARYW = 0x8A8B468C,
    MESSAGEBOXW  = 0x9ACA96AE,
};

TEST(HashTest, Ror13AnsiMixedCase) {
    EXPECT_EQ(
        ansi_ror13::LOADLIBRARYW,
        al::ror13("LoadLibraryW")
    ) << "Calculated incorrect ror13 hash of ANSI string 'LoadLibraryW'";

    EXPECT_EQ(
        ansi_ror13::LOADLIBRARYW,
        static_cast<unsigned long>("LoadLibraryW"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'LoadLibraryW'";

    EXPECT_EQ(
        ansi_ror13::MESSAGEBOXW,
        al::ror13("MessageBoxW")
    ) << "Calculated incorrect ror13 hash of ANSI string 'MessageBoxW'";

    EXPECT_EQ(
        ansi_ror13::MESSAGEBOXW,
        static_cast<unsigned long>("MessageBoxW"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'MessageBoxW'";
}

TEST(HashTest, Ror13AnsiUpperCase) {
    EXPECT_EQ(
        ansi_ror13::LOADLIBRARYW,
        al::ror13("LOADLIBRARYW")
    ) << "Calculated incorrect ror13 hash of ANSI string 'LOADLIBRARYW'";

    EXPECT_EQ(
        ansi_ror13::LOADLIBRARYW,
        static_cast<unsigned long>("LOADLIBRARYW"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'LOADLIBRARYW'";

    EXPECT_EQ(
        ansi_ror13::MESSAGEBOXW,
        al::ror13("MESSAGEBOXW")
    ) << "Calculated incorrect ror13 hash of ANSI string 'MESSAGEBOXW'";

    EXPECT_EQ(
        ansi_ror13::MESSAGEBOXW,
        static_cast<unsigned long>("MESSAGEBOXW"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'MESSAGEBOXW'";
}

TEST(HashTest, Ror13AnsiLowerCase) {
    EXPECT_EQ(
        ansi_ror13::LOADLIBRARYW,
        al::ror13("loadlibraryw")
    ) << "Calculated incorrect ror13 hash of ANSI string 'loadlibraryw'";

    EXPECT_EQ(
        ansi_ror13::LOADLIBRARYW,
        static_cast<unsigned long>("loadlibraryw"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'loadlibraryw'";

    EXPECT_EQ(
        ansi_ror13::MESSAGEBOXW,
        al::ror13("messageboxw")
    ) << "Calculated incorrect ror13 hash of ANSI string 'messageboxw'";

    EXPECT_EQ(
        ansi_ror13::MESSAGEBOXW,
        static_cast<unsigned long>("messageboxw"_ror13)
    ) << "Calculated incorrect ror13 hash of ANSI string 'messageboxw'";
}

TEST(HashTest, Ror13Utf16) {
    EXPECT_EQ(
        utf16_ror13::KERNEL32,
        al::ror13(L"KERNEL32.DLL")
    ) << "Calculated incorrect ror13 hash of UTF-16 string 'KERNEL32.DLL'";

    EXPECT_EQ(
        utf16_ror13::KERNEL32,
        static_cast<unsigned long>(L"KERNEL32.DLL"_ror13)
    ) << "Calculated incorrect ror13 hash of UTF-16 string 'KERNEL32.DLL'";

    EXPECT_EQ(
        utf16_ror13::USER32,
        al::ror13(L"USER32.DLL")
    ) << "Calculated incorrect ror13 hash of UTF-16 string 'USER32.DLL'";

    EXPECT_EQ(
        utf16_ror13::USER32,
        static_cast<unsigned long>(L"USER32.DLL"_ror13)
    ) << "Calculated incorrect ror13 hash of UTF-16 string 'USER32.DLL'";
}
