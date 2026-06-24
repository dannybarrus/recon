#include "recon/moderncpp/MoveSemanticsExamples.h"

#include <utility>

namespace recon::moderncpp {

MoveCastResult demonstrateMoveIsJustACast(std::string input) {
    MoveCastResult result;

    std::string&& castOnly = std::move(input);
    result.valueRightAfterCast = input;

    std::string destination = std::move(castOnly);
    result.originalAfterRealMove = input;
    result.movedToValue = destination;

    return result;
}

std::string whichOverload(const std::string& value) {
    return "lvalue overload (const std::string&) called with: " + value;
}

std::string whichOverload(std::string&& value) {
    return "rvalue overload (std::string&&) called with: " + value;
}

}  // namespace recon::moderncpp
