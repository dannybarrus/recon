#pragma once

namespace recon::memory {

// Demonstrates RAII's actual value proposition -- not "less typing,"
// but correctness when control flow leaves a scope in a way the
// original author didn't explicitly handle. An exception is the
// cleanest way to demonstrate this: it skips over any cleanup code
// written AFTER the point it's thrown, but it does NOT skip destructors
// of objects that were already fully constructed on the stack.
//
// Both functions simulate the same shape of bug: allocate a resource,
// then call something that might fail. If it fails, does the resource
// get cleaned up anyway?

// BROKEN: allocates with raw `new`, then calls something that may
// throw. If it throws, `delete` is never reached -- the resource leaks
// for the rest of the program's lifetime. Verified via
// TrackedResource::aliveCount() in the corresponding test, not just
// asserted in a comment.
void leaksOnException_rawPointer(bool shouldThrow);

// CORRECT: same shape, but the resource is owned by a unique_ptr. When
// shouldThrow is true and the exception propagates out of this
// function, the unique_ptr's destructor still runs during stack
// unwinding -- the resource is released regardless of how the function
// exits.
void doesNotLeakOnException_uniquePtr(bool shouldThrow);

}  // namespace recon::memory
