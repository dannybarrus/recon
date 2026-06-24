#pragma once

#include <string>

namespace recon::memory {

// Simulates a C-style resource API -- an explicit open()/close() pair,
// the shape you'd see wrapping a FILE*, a Windows HANDLE, or any legacy
// library that predates RAII conventions entirely.
//
// The point of this class is to demonstrate that std::unique_ptr is not
// only for memory allocated with `new` -- with a custom deleter, it can
// manage ANY resource that has an acquire/release pattern, even one
// that uses completely different function names than new/delete.
//
// Instrumented with an alive counter (same pattern as TrackedResource
// and GraphNode) so the custom-deleter demo can be proven rather than
// just described.
class LegacyHandle {
public:
    // Mimics a C-style factory function -- returns an owning raw
    // pointer, the same shape as fopen() or CreateFile().
    static LegacyHandle* open(const std::string& name);

    // Mimics a C-style cleanup function -- takes ownership of the
    // pointer and destroys it, the same shape as fclose() or
    // CloseHandle(). This is what gets used as unique_ptr's custom
    // deleter, instead of unique_ptr's default `delete`.
    static void close(LegacyHandle* handle);

    const std::string& name() const;

    static int aliveCount();
    static void resetAliveCount();

private:
    explicit LegacyHandle(std::string name);
    ~LegacyHandle();

    std::string name_;
    static int aliveCount_;
};

}  // namespace recon::memory
