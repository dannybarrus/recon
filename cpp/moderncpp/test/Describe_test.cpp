#include "recon/moderncpp/Describe.h"

#include <gtest/gtest.h>

namespace recon::moderncpp {
namespace {

TEST(DescribeTest, Int_UsesGenericTemplate) {
    EXPECT_EQ(describe(42), "42");
}

TEST(DescribeTest, Double_UsesGenericTemplate) {
    EXPECT_EQ(describe(3.5), "3.5");
}

TEST(DescribeTest, Bool_UsesSpecialization_NotGenericOneOrZero) {
    // The actual point of the test: without the specialization, the
    // generic ostringstream-based template would produce "1"/"0".
    EXPECT_EQ(describe(true), "true");
    EXPECT_EQ(describe(false), "false");
}

}  // namespace
}  // namespace recon::moderncpp
