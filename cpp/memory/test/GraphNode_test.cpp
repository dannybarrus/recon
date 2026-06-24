#include "recon/memory/GraphNode.h"

#include <gtest/gtest.h>
#include <memory>

namespace recon::memory {
namespace {

TEST(GraphNodeTest, NoCycle_DestroysCleanly) {
    GraphNode::resetAliveCount();
    {
        auto a = std::make_shared<GraphNode>("A");
        auto b = std::make_shared<GraphNode>("B");
        a->next = b;
        EXPECT_EQ(GraphNode::aliveCount(), 2);
    }
    EXPECT_EQ(GraphNode::aliveCount(), 0);
}

TEST(GraphNodeTest, SharedPtrCycle_Leaks) {
    // The regression test for the classic shared_ptr circular
    // reference leak. Asserted directly, not just printed in a demo --
    // unlike the timing-dependent race conditions elsewhere in this
    // repo, a shared_ptr cycle leak is 100% deterministic, so it can be
    // reliably asserted on every run.
    GraphNode::resetAliveCount();
    {
        auto a = std::make_shared<GraphNode>("A");
        auto b = std::make_shared<GraphNode>("B");
        a->next = b;
        b->next = a;
        EXPECT_EQ(GraphNode::aliveCount(), 2);
    }
    EXPECT_EQ(GraphNode::aliveCount(), 2) << "expected both nodes to leak due to the cycle";
}

TEST(GraphNodeTest, WeakPtr_BreaksTheCycle) {
    GraphNode::resetAliveCount();
    {
        auto a = std::make_shared<GraphNode>("A");
        auto b = std::make_shared<GraphNode>("B");
        a->next = b;
        b->weakNext = a;
        EXPECT_EQ(GraphNode::aliveCount(), 2);
    }
    EXPECT_EQ(GraphNode::aliveCount(), 0) << "weak_ptr should have broken the cycle";
}

TEST(GraphNodeTest, WeakPtr_LockReturnsNulloptAfterTargetDestroyed) {
    GraphNode::resetAliveCount();
    std::weak_ptr<GraphNode> weakRef;
    {
        auto a = std::make_shared<GraphNode>("Temporary");
        weakRef = a;
        EXPECT_FALSE(weakRef.expired());
    }
    EXPECT_TRUE(weakRef.expired());
    EXPECT_EQ(weakRef.lock(), nullptr);
}

TEST(GraphNodeTest, Name_ReturnsConstructorValue) {
    auto node = std::make_shared<GraphNode>("Test");
    EXPECT_EQ(node->name(), "Test");
}

}  // namespace
}  // namespace recon::memory
