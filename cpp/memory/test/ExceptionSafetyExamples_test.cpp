#include "recon/memory/ExceptionSafetyExamples.h"

#include <gtest/gtest.h>
#include <stdexcept>

#include "recon/memory/TrackedResource.h"

namespace recon::memory {
namespace {

TEST(ExceptionSafetyExamplesTest, RawPointer_NoException_DoesNotLeak) {
    TrackedResource::resetAliveCount();
    leaksOnException_rawPointer(false);
    EXPECT_EQ(TrackedResource::aliveCount(), 0);
}

TEST(ExceptionSafetyExamplesTest, RawPointer_WithException_Leaks) {
    // Regression test proving the leak is real, not just described in
    // a comment.
    TrackedResource::resetAliveCount();
    EXPECT_THROW(leaksOnException_rawPointer(true), std::runtime_error);
    EXPECT_EQ(TrackedResource::aliveCount(), 1) << "expected a leaked TrackedResource";
}

TEST(ExceptionSafetyExamplesTest, UniquePtr_NoException_DoesNotLeak) {
    TrackedResource::resetAliveCount();
    doesNotLeakOnException_uniquePtr(false);
    EXPECT_EQ(TrackedResource::aliveCount(), 0);
}

TEST(ExceptionSafetyExamplesTest, UniquePtr_WithException_StillDoesNotLeak) {
    // This is the actual point of RAII: even though an exception
    // propagates all the way out of the function, the unique_ptr's
    // destructor still ran during stack unwinding.
    TrackedResource::resetAliveCount();
    EXPECT_THROW(doesNotLeakOnException_uniquePtr(true), std::runtime_error);
    EXPECT_EQ(TrackedResource::aliveCount(), 0);
}

}  // namespace
}  // namespace recon::memory
