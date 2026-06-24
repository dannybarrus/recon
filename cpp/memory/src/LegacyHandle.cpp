#include "recon/memory/LegacyHandle.h"

namespace recon::memory {

int LegacyHandle::aliveCount_ = 0;

LegacyHandle::LegacyHandle(std::string name) : name_(std::move(name)) {
    ++aliveCount_;
}

LegacyHandle::~LegacyHandle() {
    --aliveCount_;
}

LegacyHandle* LegacyHandle::open(const std::string& name) {
    return new LegacyHandle(name);
}

void LegacyHandle::close(LegacyHandle* handle) {
    delete handle;  // private destructor -- only this function may call it
}

const std::string& LegacyHandle::name() const {
    return name_;
}

int LegacyHandle::aliveCount() {
    return aliveCount_;
}

void LegacyHandle::resetAliveCount() {
    aliveCount_ = 0;
}

}  // namespace recon::memory
