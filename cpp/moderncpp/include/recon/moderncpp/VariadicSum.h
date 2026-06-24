#pragma once

#include <type_traits>

namespace recon::moderncpp {

// Pre-C++17 technique: recursively unpack a parameter pack one
// argument at a time. Two overloads -- one for the base case (a single
// argument), one that peels off the first argument and recurses on
// whatever remains. This is how variadic templates had to be written
// before C++17.
template <typename T>
T sumRecursive(T value) {
    static_assert(std::is_arithmetic_v<T>, "sumRecursive requires arithmetic types");
    return value;
}

template <typename T, typename... Rest>
T sumRecursive(T first, Rest... rest) {
    static_assert(std::is_arithmetic_v<T>, "sumRecursive requires arithmetic types");
    return first + sumRecursive(rest...);
}

// C++17 fold expression: the modern, dramatically simpler equivalent.
// (args + ...) expands directly to first + (second + (third + ...)),
// with no recursion and no separate base-case overload required at
// all. The static_assert itself is also a fold expression --
// (std::is_arithmetic_v<Args> && ...) checks every type in the pack at
// once.
template <typename... Args>
auto sumFold(Args... args) {
    static_assert((std::is_arithmetic_v<Args> && ...), "sumFold requires arithmetic types");
    return (args + ...);
}

}  // namespace recon::moderncpp
