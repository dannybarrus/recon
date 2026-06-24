#include "recon/memory/TrackedResource.h"

namespace recon::memory {

int TrackedResource::aliveCount_ = 0;

TrackedResource::TrackedResource(std::string name) : name_(std::move(name)) {
    ++aliveCount_;
}

TrackedResource::~TrackedResource() {
    --aliveCount_;
}

TrackedResource::TrackedResource(TrackedResource&& other) noexcept
    : name_(std::move(other.name_)) {
    ++aliveCount_;
}

TrackedResource& TrackedResource::operator=(TrackedResource&& other) noexcept {
    if (this != &other) {
        name_ = std::move(other.name_);
    }
    return *this;
}

const std::string& TrackedResource::name() const {
    return name_;
}

int TrackedResource::aliveCount() {
    return aliveCount_;
}

void TrackedResource::resetAliveCount() {
    aliveCount_ = 0;
}

}  // namespace recon::memory
