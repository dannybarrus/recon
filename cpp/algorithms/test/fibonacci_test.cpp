#include "recon/algorithms/fibonacci.h"

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

TEST(FibonacciTest, Naive_BaseCases) {
    EXPECT_EQ(fibonacciNaive(0), 0);
    EXPECT_EQ(fibonacciNaive(1), 1);
}

TEST(FibonacciTest, Naive_KnownSequence) {
    int expected[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55};
    for (int i = 0; i <= 10; i++) {
        EXPECT_EQ(fibonacciNaive(i), expected[i]) << "at i=" << i;
    }
}

TEST(FibonacciTest, Iterative_BaseCases) {
    EXPECT_EQ(fibonacciIterative(0), 0);
    EXPECT_EQ(fibonacciIterative(1), 1);
}

TEST(FibonacciTest, Iterative_KnownSequence) {
    int expected[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55};
    for (int i = 0; i <= 10; i++) {
        EXPECT_EQ(fibonacciIterative(i), expected[i]) << "at i=" << i;
    }
}

TEST(FibonacciTest, NaiveAndIterative_AgreeForLargerValues) {
    for (int i = 0; i <= 25; i++) {
        EXPECT_EQ(fibonacciNaive(i), fibonacciIterative(i)) << "at i=" << i;
    }
}

}  // namespace
}  // namespace recon::algorithms
