#include "imports.hpp"
#include <al/al.hpp>
#include <al/gtest/pic.hpp>

struct ImportTest : PICTest<> {};

TEST_F(ImportTest, Imports) {
    results r{};

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    ASSERT_NE(nullptr, ntdll);

    FARPROC unload_dll = GetProcAddress(ntdll, "LdrUnloadDll");
    ASSERT_NE(nullptr, unload_dll);

    pic(&r);

    EXPECT_EQ(static_cast<void*>(ntdll), r.module_name);
    EXPECT_EQ(static_cast<void*>(ntdll), r.module_ror13);
    EXPECT_EQ(static_cast<void*>(ntdll), r.module_djb2);

    EXPECT_EQ(static_cast<void*>(unload_dll), r.import_name);
    EXPECT_EQ(static_cast<void*>(unload_dll), r.import_ror13);
    EXPECT_EQ(static_cast<void*>(unload_dll), r.import_djb2);
}
