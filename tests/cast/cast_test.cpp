#include "cast.hpp"
#include <al/gtest/pic.hpp>

struct CastTest : public PICTest<CastTest> {
    inline static std::string_view path = CAST_PIC_PATH;
    inline static unsigned long permissions = PAGE_EXECUTE_READWRITE;
    cast_t m_pic = nullptr;
};

TEST_F(CastTest, FunctionCast) {
    ASSERT_EQ(m_pic(), 0);
}
