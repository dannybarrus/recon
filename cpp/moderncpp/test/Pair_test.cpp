#include "recon/moderncpp/Pair.h"

#include <gtest/gtest.h>
#include <string>

namespace recon::moderncpp {
namespace {

TEST(PairTest, FirstAndSecond_ReturnConstructorValues) {
    Pair<int, std::string> p(42, "answer");
    EXPECT_EQ(p.first(), 42);
    EXPECT_EQ(p.second(), "answer");
}

TEST(PairTest, WorksWithCompletelyDifferentTypeCombinations) {
    Pair<double, bool> p(3.14, true);
    EXPECT_DOUBLE_EQ(p.first(), 3.14);
    EXPECT_TRUE(p.second());
}

TEST(PairTest, Equality_ComparesBothElements) {
    Pair<int, int> a(1, 2);
    Pair<int, int> b(1, 2);
    Pair<int, int> c(1, 3);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(PairTest, Swap_ExchangesContents) {
    Pair<int, std::string> a(1, "a");
    Pair<int, std::string> b(2, "b");

    a.swap(b);

    EXPECT_EQ(a.first(), 2);
    EXPECT_EQ(a.second(), "b");
    EXPECT_EQ(b.first(), 1);
    EXPECT_EQ(b.second(), "a");
}

TEST(MakePairTest, DeducesTypesFromArguments) {
    auto p = makePair(3.14, true);
    EXPECT_DOUBLE_EQ(p.first(), 3.14);
    EXPECT_TRUE(p.second());
}

}  // namespace
}  // namespace recon::moderncpp
