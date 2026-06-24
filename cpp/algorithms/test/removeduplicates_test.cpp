#include "recon/algorithms/removeduplicates.h"

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

TEST(RemoveDuplicatesTest, OriginalRepoTestCase) {
    int arr[] = {1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 8, 9, 0};
    int size = static_cast<int>(sizeof(arr) / sizeof(arr[0]));

    int newLength = removeDuplicates(arr, size);

    EXPECT_EQ(newLength, 10);
    int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    for (int i = 0; i < newLength; i++) {
        EXPECT_EQ(arr[i], expected[i]) << "at index " << i;
    }
}

TEST(RemoveDuplicatesTest, NoDuplicates_UnchangedLength) {
    int arr[] = {1, 2, 3, 4, 5};
    int newLength = removeDuplicates(arr, 5);
    EXPECT_EQ(newLength, 5);
}

TEST(RemoveDuplicatesTest, AllSameElement) {
    int arr[] = {7, 7, 7, 7, 7};
    int newLength = removeDuplicates(arr, 5);
    EXPECT_EQ(newLength, 1);
    EXPECT_EQ(arr[0], 7);
}

TEST(RemoveDuplicatesTest, SingleElement) {
    int arr[] = {42};
    int newLength = removeDuplicates(arr, 1);
    EXPECT_EQ(newLength, 1);
    EXPECT_EQ(arr[0], 42);
}

TEST(RemoveDuplicatesTest, EmptyArray) {
    int arr[] = {0};
    int newLength = removeDuplicates(arr, 0);
    EXPECT_EQ(newLength, 0);
}

}  // namespace
}  // namespace recon::algorithms
