#pragma once

#include <string>

namespace recon::moderncpp {

// Result of demonstrateMoveIsJustACast() -- three observable states
// that make the actual lesson testable, not just asserted in a comment.
struct MoveCastResult {
    std::string valueRightAfterCast;    // should equal the original input -- nothing has moved yet
    std::string originalAfterRealMove;  // the moved-from state, AFTER something actually consumed the rvalue reference
    std::string movedToValue;           // the destination that received the moved value
};

// THE key misconception this corrects: std::move(x) does not move
// anything by itself. It is only a cast to an rvalue reference -- a way
// of saying "this value is eligible to be moved from." The actual move
// only happens later, at the point some move constructor or move
// assignment operator actually consumes that rvalue reference.
//
// Note on moved-from state: for std::string specifically, common
// standard library implementations (libstdc++, libc++) leave a
// moved-from string empty in practice. The C++ standard itself only
// guarantees a moved-from std::string is left in SOME valid but
// otherwise unspecified state -- code should never rely on a specific
// moved-from value being empty, even though it usually is in practice.
MoveCastResult demonstrateMoveIsJustACast(std::string input);

// Overloaded on lvalue reference vs rvalue reference. Returns a string
// identifying which overload was actually selected, so the difference
// in overload resolution is directly observable rather than just
// described.
std::string whichOverload(const std::string& value);
std::string whichOverload(std::string&& value);

}  // namespace recon::moderncpp
