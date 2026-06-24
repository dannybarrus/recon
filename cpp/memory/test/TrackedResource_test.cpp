#include "recon/memory/TrackedResource.h"

#include <gtest/gtest.h>
#include <utility>

namespace recon::memory {
namespace {

TEST(TrackedResourceTest, ConstructionIncrementsAliveCount) {
    TrackedResource::resetAliveCount();
    EXPECT_EQ(TrackedResource::aliveCount(), 0);
    {
        TrackedResource r("a");
        EXPECT_EQ(TrackedResource::aliveCount(), 1);
    }
    EXPECT_EQ(TrackedResource::aliveCount(), 0);
}

TEST(TrackedResourceTest, MultipleInstances_CountIndependently) {
    TrackedResource::resetAliveCount();
    {
        TrackedResource a("a");
        TrackedResource b("b");
        TrackedResource c("c");
        EXPECT_EQ(TrackedResource::aliveCount(), 3);
    }
    EXPECT_EQ(TrackedResource::aliveCount(), 0);
}

TEST(TrackedResourceTest, MoveConstruction_CountsAsANewObject) {
    TrackedResource::resetAliveCount();
    {
        TrackedResource original("movable");
        EXPECT_EQ(TrackedResource::aliveCount(), 1);

        TrackedResource moved(std::move(original));
        // Both the moved-from and moved-to objects still exist and will
        // each run their own destructor -- so the count is 2 here, not 1.
        EXPECT_EQ(TrackedResource::aliveCount(), 2);
    }
    EXPECT_EQ(TrackedResource::aliveCount(), 0);
}

TEST(TrackedResourceTest, Name_ReturnsConstructorValue) {
    TrackedResource r("hello");
    EXPECT_EQ(r.name(), "hello");
}

TEST(TrackedResourceTest, ResetAliveCount_ZeroesTheCounter) {
    TrackedResource::resetAliveCount();
    {
        TrackedResource a("a");
        EXPECT_EQ(TrackedResource::aliveCount(), 1);
    }
    // a's destructor already ran by this point -- verify reset still
    // works cleanly when the count is already back to 0.
    TrackedResource::resetAliveCount();
    EXPECT_EQ(TrackedResource::aliveCount(), 0);
}

}  // namespace
}  // namespace recon::memory
