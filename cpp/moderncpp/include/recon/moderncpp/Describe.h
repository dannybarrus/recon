#pragma once

#include <sstream>
#include <string>

namespace recon::moderncpp {

// A function template with an explicit full specialization -- the
// primary template handles the general case, and the specialization
// for T = bool overrides it with different, more useful behavior for
// that one specific type.
//
// Generic version: stream the value through ostringstream. Works for
// any type with an operator<<, including bool -- but for bool,
// operator<< prints "1"/"0" by default (without std::boolalpha set),
// which is correct, standard behavior but not very readable.
template <typename T>
std::string describe(const T& value) {
    std::ostringstream out;
    out << value;
    return out.str();
}

// Full specialization for bool: same signature, a completely different
// implementation, selected by the compiler automatically whenever
// describe() is called with a bool argument -- callers never need to
// know the specialization exists. Marked inline because, unlike the
// primary template (implicitly inline when defined in a header), an
// explicit specialization is a concrete, non-template function and
// would violate the one-definition rule if this header were ever
// included into more than one translation unit within the same binary
// without it.
template <>
inline std::string describe<bool>(const bool& value) {
    return value ? "true" : "false";
}

}  // namespace recon::moderncpp
