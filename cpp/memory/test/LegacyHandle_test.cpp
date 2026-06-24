#include "recon/memory/LegacyHandle.h"

#include <gtest/gtest.h>
#include <memory>

namespace recon::memory {
namespace {

TEST(LegacyHandleTest, OpenIncrementsAliveCount) {
    LegacyHandle::resetAliveCount();
    LegacyHandle* handle = LegacyHandle::open("test.dat");
    EXPECT_EQ(LegacyHandle::aliveCount(), 1);
    LegacyHandle::close(handle);
    EXPECT_EQ(LegacyHandle::aliveCount(), 0);
}

TEST(LegacyHandleTest, Name_ReturnsOpenArgument) {
    LegacyHandle::resetAliveCount();
    LegacyHandle* handle = LegacyHandle::open("config.dat");
    EXPECT_EQ(handle->name(), "config.dat");
    LegacyHandle::close(handle);
}

TEST(LegacyHandleTest, UniquePtrWithCustomDeleter_ClosesAutomatically) {
    LegacyHandle::resetAliveCount();
    {
        std::unique_ptr<LegacyHandle, decltype(&LegacyHandle::close)> handle(
            LegacyHandle::open("scoped.dat"), &LegacyHandle::close);
        EXPECT_EQ(LegacyHandle::aliveCount(), 1);
        EXPECT_EQ(handle->name(), "scoped.dat");
    }
    EXPECT_EQ(LegacyHandle::aliveCount(), 0);
}

TEST(LegacyHandleTest, MultipleHandles_CountIndependently) {
    LegacyHandle::resetAliveCount();
    LegacyHandle* a = LegacyHandle::open("a.dat");
    LegacyHandle* b = LegacyHandle::open("b.dat");
    EXPECT_EQ(LegacyHandle::aliveCount(), 2);
    LegacyHandle::close(a);
    EXPECT_EQ(LegacyHandle::aliveCount(), 1);
    LegacyHandle::close(b);
    EXPECT_EQ(LegacyHandle::aliveCount(), 0);
}

}  // namespace
}  // namespace recon::memory
