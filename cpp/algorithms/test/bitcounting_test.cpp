#include "recon/algorithms/bitcounting.h"

#include <gtest/gtest.h>

namespace recon::algorithms {
namespace {

class BitcountingTest : public ::testing::Test {
protected:
    void SetUp() override { computeBitcountTables(); }
};

TEST_F(BitcountingTest, IteratedBitcount_Zero) {
    EXPECT_EQ(iteratedBitcount(0), 0);
}

TEST_F(BitcountingTest, IteratedBitcount_AllOnes) {
    EXPECT_EQ(iteratedBitcount(0xFFFFFFFFu), 32);
}

TEST_F(BitcountingTest, IteratedBitcount_SingleBit) {
    EXPECT_EQ(iteratedBitcount(0x1u), 1);
    EXPECT_EQ(iteratedBitcount(0x80000000u), 1);
}

TEST_F(BitcountingTest, IteratedBitcount_KnownValue) {
    EXPECT_EQ(iteratedBitcount(0xDEADBEEFu), 24);
}

TEST_F(BitcountingTest, AllImplementations_AgreeOnZero) {
    EXPECT_TRUE(verifyBitcounts(0));
}

TEST_F(BitcountingTest, AllImplementations_AgreeOnAllOnes) {
    EXPECT_TRUE(verifyBitcounts(0xFFFFFFFFu));
}

TEST_F(BitcountingTest, AllImplementations_AgreeOnKnownValue) {
    EXPECT_TRUE(verifyBitcounts(0xDEADBEEFu));
}

TEST_F(BitcountingTest, AllImplementations_AgreeAcrossManyValues) {
    for (unsigned int i = 0; i < 5000; i++) {
        EXPECT_TRUE(verifyBitcounts(i * 7919u)) << "mismatch at i=" << i;
    }
}

}  // namespace
}  // namespace recon::algorithms
