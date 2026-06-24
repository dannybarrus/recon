#include "recon/algorithms/strintintstr.h"

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

TEST(StrIntIntStrTest, PositiveNumber) {
    char buf[64];
    intToStr(14352, buf);
    EXPECT_STREQ(buf, "14352");
    EXPECT_EQ(strToInt(buf), 14352);
}

TEST(StrIntIntStrTest, Zero) {
    char buf[64];
    intToStr(0, buf);
    EXPECT_STREQ(buf, "0");
    EXPECT_EQ(strToInt(buf), 0);
}

TEST(StrIntIntStrTest, NegativeNumber) {
    char buf[64];
    intToStr(-42, buf);
    EXPECT_STREQ(buf, "-42");
    EXPECT_EQ(strToInt(buf), -42);
}

TEST(StrIntIntStrTest, One) {
    char buf[64];
    intToStr(1, buf);
    EXPECT_STREQ(buf, "1");
    EXPECT_EQ(strToInt(buf), 1);
}

TEST(StrIntIntStrTest, NegativeOne) {
    char buf[64];
    intToStr(-1, buf);
    EXPECT_STREQ(buf, "-1");
    EXPECT_EQ(strToInt(buf), -1);
}

TEST(StrIntIntStrTest, RoundTrip_RangeOfValues) {
    int values[] = {1, -1, 100, -100, 999999, -999999, 7, -7};
    for (int v : values) {
        char buf[64];
        intToStr(v, buf);
        EXPECT_EQ(strToInt(buf), v) << "round-trip failed for " << v;
    }
}

// NOTE: INT_MIN is intentionally not tested here for the negation
// round-trip -- negating INT_MIN is undefined behavior, documented in
// the header rather than silently patched over.

}  // namespace
}  // namespace recon::algorithms
