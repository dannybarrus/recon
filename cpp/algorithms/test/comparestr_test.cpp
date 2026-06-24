#include "recon/algorithms/comparestr.h"

#include <cstring>

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

TEST(CompareStringsTest, IdenticalStrings_ReturnsZero) {
    EXPECT_EQ(compareStrings("Test1", "Test1"), 0);
}

TEST(CompareStringsTest, DifferentStrings_ReturnsNonZero) {
    EXPECT_NE(compareStrings("Test1 this does not match", "Test1 tnope it sure does not"), 0);
}

TEST(CompareStringsTest, FirstStringSortsEarlier_ReturnsNegative) {
    EXPECT_LT(compareStrings("apple", "banana"), 0);
}

TEST(CompareStringsTest, FirstStringSortsLater_ReturnsPositive) {
    EXPECT_GT(compareStrings("banana", "apple"), 0);
}

TEST(CompareStringsTest, EmptyStrings_ReturnsZero) {
    EXPECT_EQ(compareStrings("", ""), 0);
}

TEST(CompareStringsTest, OneEmptyOneNot) {
    EXPECT_LT(compareStrings("", "a"), 0);
    EXPECT_GT(compareStrings("a", ""), 0);
}

TEST(CompareStringsTest, MatchesStandardStrcmpBehavior) {
    const char* a = "hello";
    const char* b = "help";
    int ours = compareStrings(a, b);
    int real = std::strcmp(a, b);
    EXPECT_EQ((ours < 0), (real < 0));
    EXPECT_EQ((ours > 0), (real > 0));
    EXPECT_EQ((ours == 0), (real == 0));
}

}  // namespace
}  // namespace recon::algorithms
