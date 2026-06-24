#include "recon/algorithms/expressionmatch.h"

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

constexpr const char* kText = "This is a test string";

TEST(ExpressionMatchTest, OriginalRepoTestCases) {
    EXPECT_TRUE(matchesPattern(kText, "T*test*string"));
    EXPECT_TRUE(matchesPattern(kText, "*test*"));
    EXPECT_TRUE(matchesPattern(kText, "*"));
    EXPECT_TRUE(matchesPattern(kText, "*This is *"));
    EXPECT_FALSE(matchesPattern(kText, "A*string"));
    EXPECT_TRUE(matchesPattern(kText, "This is a test string"));
    EXPECT_FALSE(matchesPattern(kText, "This is a test"));
    EXPECT_TRUE(matchesPattern(kText, "This is***"));
}

TEST(ExpressionMatchTest, TheBigBug_SegmentAfterWildcardMustActuallyMatch) {
    // This is THE test that the original implementation could not pass.
    // The literal text before the wildcard ("This") is present in kText,
    // but "ZZZNOTPRESENT" after the wildcard is not present anywhere.
    // The original returned true here -- it never validated anything
    // after the first wildcard at all.
    EXPECT_FALSE(matchesPattern(kText, "This*ZZZNOTPRESENT"));
}

TEST(ExpressionMatchTest, MultipleWildcardsWithRealSegmentsBetweenThem) {
    EXPECT_TRUE(matchesPattern(kText, "This*a*string"));
    EXPECT_FALSE(matchesPattern(kText, "This*notpresent*string"));
}

TEST(ExpressionMatchTest, WildcardAtVeryStartAndVeryEnd) {
    EXPECT_TRUE(matchesPattern(kText, "*is a test*"));
    EXPECT_FALSE(matchesPattern(kText, "*is a missing thing*"));
}

TEST(ExpressionMatchTest, EmptyPatternMatchesOnlyEmptyText) {
    EXPECT_TRUE(matchesPattern("", ""));
    EXPECT_FALSE(matchesPattern("nonempty", ""));
}

TEST(ExpressionMatchTest, SingleWildcardMatchesEmptyText) {
    EXPECT_TRUE(matchesPattern("", "*"));
}

TEST(ExpressionMatchTest, NoWildcards_ExactMatchOnly) {
    EXPECT_TRUE(matchesPattern("exact", "exact"));
    EXPECT_FALSE(matchesPattern("exact", "exacto"));
    EXPECT_FALSE(matchesPattern("exact", "exac"));
}

TEST(ExpressionMatchTest, ConsecutiveWildcards_BehaveAsOne) {
    EXPECT_TRUE(matchesPattern(kText, "This****string"));
}

}  // namespace
}  // namespace recon::algorithms
