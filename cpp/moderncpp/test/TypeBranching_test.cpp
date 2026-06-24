#include "recon/moderncpp/TypeBranching.h"

#include <gtest/gtest.h>
#include <string>

namespace recon::moderncpp {
namespace {

TEST(TypeBranchingTest, ArithmeticType_TakesArithmeticBranch) {
    EXPECT_EQ(describeKind(42), "arithmetic value");
    EXPECT_EQ(describeKind(3.14), "arithmetic value");
}

TEST(TypeBranchingTest, NonNullPointer_TakesPointerBranch) {
    int value = 10;
    int* ptr = &value;
    EXPECT_EQ(describeKind(ptr), "pointer (non-null)");
}

TEST(TypeBranchingTest, NullPointer_TakesPointerBranchWithNullCase) {
    int* nullPtr = nullptr;
    EXPECT_EQ(describeKind(nullPtr), "pointer (null)");
}

TEST(TypeBranchingTest, OtherType_TakesElseBranch) {
    std::string text = "hello";
    EXPECT_EQ(describeKind(text), "some other type");
}

}  // namespace
}  // namespace recon::moderncpp
