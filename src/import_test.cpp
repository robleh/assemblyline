#include <gtest/gtest.h>
#include <windows.h>
#include <al/al.hpp>

class ImportTest : public testing::Test {
protected:
    void* m_ntdll;
    void* m_kernel32;

    void SetUp() override {
        m_ntdll    = GetModuleHandleW(L"ntdll.dll");
        ASSERT_TRUE(m_ntdll) << "Windows failed to resolved ntdll";

        m_kernel32 = GetModuleHandleW(L"kernel32.dll");
        ASSERT_TRUE(m_kernel32) << "Windows failed to resolved ntdll";
    }

    void TearDown() override {
    }
};

TEST_F(ImportTest, ModuleUppercase) {
    void* al_ntdll = al::get_module_handle(L"NTDLL.DLL");
    EXPECT_EQ(
        m_ntdll, 
        al_ntdll
    ) << "Uppercase search for 'NTDLL.DLL' failed";

    void* al_kernel32 = al::get_module_handle(L"KERNEL32.DLL");
    EXPECT_EQ(
        m_kernel32,
        al_kernel32
    ) << "Uppercase search for 'KERNEL32.DLL' failed";
}

TEST_F(ImportTest, ModuleLowercase) {
    void* al_ntdll = al::get_module_handle(L"ntdll.dll");
    EXPECT_EQ(
        m_ntdll,
        al_ntdll
    ) << "Lowercase search for 'ntdll.dll' failed";

    void* al_kernel32 = al::get_module_handle(L"kernel32.dll");
    EXPECT_EQ(
        m_kernel32,
        al_kernel32
    ) << "Lowercase search for 'kernel32.dll' failed";
}

TEST_F(ImportTest, ProcMixedcase) {
    void* al_gpa = al::get_proc_address(m_kernel32, "GetProcAddress");
    EXPECT_EQ(
        GetProcAddress,
        al_gpa
    ) << "Resolution of export name 'GetProcAddress' failed";
}

TEST_F(ImportTest, ProcNonExistent) {
    void* win_gpa = GetProcAddress(reinterpret_cast<HMODULE>(m_kernel32), "getprocaddress");
    EXPECT_FALSE(
        win_gpa
    ) << "Windows unexpectedly resolved export name 'getprocaddress'";

    void* al_gpa = al::get_proc_address(m_kernel32, "getprocaddress");
    EXPECT_EQ(
        win_gpa,
        al_gpa
    ) << "Assembly Line unexpectedly resolved export name 'getprocaddress'";
}
