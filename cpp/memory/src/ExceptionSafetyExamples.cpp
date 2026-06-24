#include "recon/memory/ExceptionSafetyExamples.h"

#include <memory>
#include <stdexcept>

#include "recon/memory/TrackedResource.h"

namespace recon::memory {

namespace {
void mayThrow(bool shouldThrow) {
    if (shouldThrow) {
        throw std::runtime_error("simulated failure");
    }
}
}  // namespace

void leaksOnException_rawPointer(bool shouldThrow) {
    TrackedResource* resource = new TrackedResource("raw");
    mayThrow(shouldThrow);
    delete resource;
}

void doesNotLeakOnException_uniquePtr(bool shouldThrow) {
    std::unique_ptr<TrackedResource> resource = std::make_unique<TrackedResource>("unique_ptr");
    mayThrow(shouldThrow);
}

}  // namespace recon::memory
