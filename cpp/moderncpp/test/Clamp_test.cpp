#include "recon/moderncpp/Clamp.h"

#include <gtest/gtest.h>

namespace recon::moderncpp {
namespace {

TEST(ClampTest, ValueAboveRange_ReturnsHigh) {
    EXPECT_EQ(clamp(15, 0, 10), 10);
}

TEST(ClampTest, ValueBelowRange_ReturnsLow) {
    EXPECT_EQ(clamp(-5, 0, 10), 0);
}

TEST(ClampTest, ValueWithinRange_ReturnsValueUnchanged) {
    EXPECT_EQ(clamp(5, 0, 10), 5);
}

TEST(ClampTest, ValueAtBoundaries_ReturnsValueUnchanged) {
    EXPECT_EQ(clamp(0, 0, 10), 0);
    EXPECT_EQ(clamp(10, 0, 10), 10);
}

TEST(ClampTest, WorksWithDoubles) {
    EXPECT_DOUBLE_EQ(clamp(2.5, 0.0, 10.0), 2.5);
    EXPECT_DOUBLE_EQ(clamp(-1.0, 0.0, 10.0), 0.0);
    EXPECT_DOUBLE_EQ(clamp(99.0, 0.0, 10.0), 10.0);
}

}  // namespace
}  // namespace recon::moderncpp
