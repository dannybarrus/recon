#include "recon/algorithms/custom_strchr.h"

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

TEST(CustomStrchrTest, MatchAtIndexZero_TheOriginalBug) {
    // This is the exact case the original implementation got wrong --
    // mystrchr("abc", 'a') incorrectly returned nullptr.
    const char* result = customStrchr("abc", 'a');
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "abc");
}

TEST(CustomStrchrTest, MatchInMiddle) {
    const char* result = customStrchr("abc", 'b');
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "bc");
}

TEST(CustomStrchrTest, MatchAtEnd) {
    const char* result = customStrchr("abc", 'c');
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "c");
}

TEST(CustomStrchrTest, NoMatch_ReturnsNullptr) {
    EXPECT_EQ(customStrchr("abc", 'z'), nullptr);
}

TEST(CustomStrchrTest, OriginalRepoTestCase) {
    const char* result = customStrchr("This is a test", 't');
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "test");
}

TEST(CustomStrchrTest, SearchForNullTerminator_MatchesStandardBehavior) {
    const char* s = "abc";
    const char* result = customStrchr(s, '\0');
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(*result, '\0');
}

TEST(CustomStrchrTest, EmptyString_OnlyMatchesNullTerminator) {
    EXPECT_EQ(customStrchr("", 'a'), nullptr);
    EXPECT_NE(customStrchr("", '\0'), nullptr);
}

}  // namespace
}  // namespace recon::algorithms
