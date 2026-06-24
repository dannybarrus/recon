#pragma once

#include <memory>
#include <string>

namespace recon::memory {

// Demonstrates the classic shared_ptr circular-reference leak, and the
// fix with weak_ptr.
//
// shared_ptr uses reference counting: an object is destroyed when its
// reference count reaches zero. If two objects hold shared_ptrs to each
// other, each one's reference count includes the OTHER object's
// reference -- so even after every reference from OUTSIDE the cycle
// goes away, each object still holds one reference from inside the
// cycle, and the count never reaches zero. Neither destructor ever
// runs. This is a real, silent memory leak with no warning from the
// compiler or from shared_ptr itself.
//
// `next` is a strong (shared_ptr) reference -- this is what CAUSES the
// cycle when used in both directions. `weakNext` is a weak reference --
// it does not contribute to the reference count at all, so wiring one
// direction of a cycle through weakNext instead of next breaks the
// cycle entirely.
//
// Both the leak and the fix are proven via TrackedResource-style
// instrumentation (aliveCount()), not just asserted in prose -- see
// GraphNode_test.cpp, where the leaked case is asserted to NOT return
// to zero, and the fixed case is asserted to return to exactly zero.
class GraphNode {
public:
    explicit GraphNode(std::string name);
    ~GraphNode();

    const std::string& name() const;

    std::shared_ptr<GraphNode> next;
    std::weak_ptr<GraphNode> weakNext;

    static int aliveCount();
    static void resetAliveCount();

private:
    std::string name_;
    static int aliveCount_;
};

}  // namespace recon::memory
