#include "recon/moderncpp/Buffer.h"

#include <gtest/gtest.h>
#include <utility>

namespace recon::moderncpp {
namespace {

TEST(BufferTest, ConstructorFillsWithExpectedPattern) {
    Buffer b(5);
    for (std::size_t i = 0; i < 5; i++) {
        EXPECT_EQ(b.data()[i], static_cast<double>(i));
    }
}

TEST(BufferTest, Size_ReturnsConstructorValue) {
    Buffer b(42);
    EXPECT_EQ(b.size(), 42u);
}

TEST(BufferTest, CopyConstructor_IncrementsCopyCount) {
    Buffer::resetCounts();
    Buffer original(10);
    Buffer copy(original);
    EXPECT_EQ(Buffer::copyCount(), 1);
    EXPECT_EQ(Buffer::moveCount(), 0);
}

TEST(BufferTest, CopyConstructor_ProducesIndependentData) {
    Buffer original(5);
    Buffer copy(original);

    EXPECT_EQ(copy.size(), original.size());
    EXPECT_NE(copy.data(), original.data());

    copy.data()[0] = 999.0;
    EXPECT_NE(original.data()[0], 999.0);
}

TEST(BufferTest, MoveConstructor_IncrementsMoveCount) {
    Buffer::resetCounts();
    Buffer original(10);
    Buffer moved(std::move(original));
    EXPECT_EQ(Buffer::moveCount(), 1);
    EXPECT_EQ(Buffer::copyCount(), 0);
}

TEST(BufferTest, MoveConstructor_LeavesSourceEmpty) {
    Buffer original(10);
    Buffer moved(std::move(original));

    EXPECT_EQ(moved.size(), 10u);
    EXPECT_EQ(original.size(), 0u);
    EXPECT_EQ(original.data(), nullptr);
}

TEST(BufferTest, MoveAssignment_TransfersOwnership) {
    Buffer::resetCounts();
    Buffer target(3);
    Buffer source(10);

    target = std::move(source);

    EXPECT_EQ(target.size(), 10u);
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(Buffer::moveCount(), 1);
}

TEST(BufferTest, CopyAssignment_IncrementsCopyCount) {
    Buffer::resetCounts();
    Buffer target(3);
    Buffer source(10);

    target = source;

    EXPECT_EQ(target.size(), 10u);
    EXPECT_EQ(Buffer::copyCount(), 1);
}

TEST(BufferTest, ResetCounts_ZeroesBothCounters) {
    Buffer a(5);
    Buffer b(a);
    Buffer c(std::move(a));
    Buffer::resetCounts();
    EXPECT_EQ(Buffer::copyCount(), 0);
    EXPECT_EQ(Buffer::moveCount(), 0);
}

}  // namespace
}  // namespace recon::moderncpp
