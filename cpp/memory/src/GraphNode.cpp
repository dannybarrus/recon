#include "recon/memory/GraphNode.h"

namespace recon::memory {

int GraphNode::aliveCount_ = 0;

GraphNode::GraphNode(std::string name) : name_(std::move(name)) {
    ++aliveCount_;
}

GraphNode::~GraphNode() {
    --aliveCount_;
}

const std::string& GraphNode::name() const {
    return name_;
}

int GraphNode::aliveCount() {
    return aliveCount_;
}

void GraphNode::resetAliveCount() {
    aliveCount_ = 0;
}

}  // namespace recon::memory
