#pragma once

#include <string>

namespace recon::memory {

// An instrumented resource type: every constructor increments a static
// counter, every destructor decrements it. This is the proof mechanism
// for everything else in this folder -- the same pattern as
// LazyEvaluation's execution trace and ManagedResource's open/close
// trace elsewhere in this repo. None of the RAII or smart-pointer
// lessons here are taken on faith; aliveCount() lets every demo and
// test directly observe whether cleanup actually happened.
//
// Copy is disabled (this is a teaching vehicle, not a value type that
// needs to be duplicated). Move is provided and explicitly increments
// the counter -- a move constructs a genuinely new C++ object (the
// moved-from object still exists and will still run its own destructor
// later), so it must count as a new "alive" object for the bookkeeping
// to stay balanced.
class TrackedResource {
public:
    explicit TrackedResource(std::string name);
    ~TrackedResource();

    TrackedResource(const TrackedResource&) = delete;
    TrackedResource& operator=(const TrackedResource&) = delete;

    TrackedResource(TrackedResource&& other) noexcept;
    TrackedResource& operator=(TrackedResource&& other) noexcept;

    const std::string& name() const;

    // Number of TrackedResource objects currently alive (constructed
    // but not yet destructed), across the whole program.
    static int aliveCount();

    // Resets the counter to 0. Used between independent test cases so
    // one test's resources never pollute another's expected count.
    static void resetAliveCount();

private:
    std::string name_;
    static int aliveCount_;
};

}  // namespace recon::memory
