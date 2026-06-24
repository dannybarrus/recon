#include "recon/moderncpp/VariadicSum.h"

#include <gtest/gtest.h>

namespace recon::moderncpp {
namespace {

TEST(VariadicSumTest, SumRecursive_SingleArgument) {
    EXPECT_EQ(sumRecursive(5), 5);
}

TEST(VariadicSumTest, SumRecursive_MultipleArguments) {
    EXPECT_EQ(sumRecursive(1, 2, 3, 4), 10);
}

TEST(VariadicSumTest, SumFold_SingleArgument) {
    EXPECT_EQ(sumFold(5), 5);
}

TEST(VariadicSumTest, SumFold_MultipleArguments) {
    EXPECT_EQ(sumFold(1, 2, 3, 4), 10);
}

TEST(VariadicSumTest, SumFold_Doubles) {
    EXPECT_DOUBLE_EQ(sumFold(1.5, 2.5, 3.0), 7.0);
}

TEST(VariadicSumTest, BothTechniques_ProduceIdenticalResults) {
    EXPECT_EQ(sumRecursive(1, 2, 3, 4, 5), sumFold(1, 2, 3, 4, 5));
}

}  // namespace
}  // namespace recon::moderncpp
