#include "recon/algorithms/reversestring.h"

#include <cstring>

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

TEST(ReverseStringTest, SimpleWord) {
    char buf[] = "hello";
    reverseString(buf);
    EXPECT_STREQ(buf, "olleh");
}

TEST(ReverseStringTest, Sentence) {
    char buf[] = "This is a test";
    reverseString(buf);
    EXPECT_STREQ(buf, "tset a si sihT");
}

TEST(ReverseStringTest, SingleCharacter) {
    char buf[] = "x";
    reverseString(buf);
    EXPECT_STREQ(buf, "x");
}

TEST(ReverseStringTest, EmptyString) {
    char buf[] = "";
    reverseString(buf);
    EXPECT_STREQ(buf, "");
}

TEST(ReverseStringTest, ReturnsTheSamePointer) {
    char buf[] = "abc";
    char* result = reverseString(buf);
    EXPECT_EQ(result, buf);
}

TEST(ReverseStringTest, ContainingPercentCharacters_DoesNotCrashOrCorrupt) {
    // This is exactly the case that would have triggered the original's
    // printf(szResult) format-string bug if it still passed the reversed
    // string directly as a format string. As long as this test passes
    // and nothing crashes, the fix (printf("%s", ...) at the call site)
    // is doing its job -- the reversal itself just operates on data.
    char buf[] = "100%% off";
    reverseString(buf);
    EXPECT_STREQ(buf, "ffo %%001");
}

}  // namespace
}  // namespace recon::algorithms
