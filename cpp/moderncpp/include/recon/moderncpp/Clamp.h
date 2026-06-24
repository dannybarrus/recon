#pragma once

namespace recon::moderncpp {

// A function template -- one definition, instantiated for whatever
// type T is actually used at each call site (int, double, a custom
// type with operator<, etc.). The compiler generates a separate
// clamp<int>, clamp<double>, and so on, each time a new T is used.
//
// This predates std::clamp (added in C++17) in spirit -- written here
// to keep the mechanism visible. Requires T to support operator<,
// which every built-in numeric type does, and which any custom type
// can opt into by defining its own operator<.
template <typename T>
const T& clamp(const T& value, const T& low, const T& high) {
    if (value < low) return low;
    if (high < value) return high;
    return value;
}

}  // namespace recon::moderncpp
