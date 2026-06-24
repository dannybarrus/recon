#include "recon/algorithms/contigarray.h"

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

TEST(ContigArrayTest, FullyAscending) {
    int arr[] = {-3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8};
    EXPECT_EQ(countContiguousElements(arr, 12), 12);
}

TEST(ContigArrayTest, FullyDescending) {
    int arr[] = {3, 2, 1, 0, -1, -2, -3, -4, -5, -6, -7, -8};
    EXPECT_EQ(countContiguousElements(arr, 12), 12);
}

TEST(ContigArrayTest, AscendingThenDescending_TakesTheLongerRun) {
    int arr[] = {1, 2, 3, 4, 5, -2, -3, -4, -5, -6, -7, -8};
    EXPECT_EQ(countContiguousElements(arr, 12), 7);
}

TEST(ContigArrayTest, SingleElement_RunOfOne) {
    int arr[] = {42};
    EXPECT_EQ(countContiguousElements(arr, 1), 1);
}

TEST(ContigArrayTest, NoContiguousRun_EveryElementIsolated) {
    int arr[] = {10, 50, 3, 99, 1};
    EXPECT_EQ(countContiguousElements(arr, 5), 1);
}

TEST(ContigArrayTest, EmptyArray) {
    int arr[] = {0};
    EXPECT_EQ(countContiguousElements(arr, 0), 0);
}

TEST(ContigArrayTest, DoesNotReadPastTheEndOfTheArray) {
    // Regression test for the AddressSanitizer-confirmed buffer overflow.
    // Build and run this test suite with -DRECON_ASAN=ON to have ASan
    // actively verify there is no out-of-bounds read here -- the
    // original would have aborted on exactly this call.
    int arr[] = {1, 2, 3, 4, 5};
    EXPECT_EQ(countContiguousElements(arr, 5), 5);
}

}  // namespace
}  // namespace recon::algorithms
