#pragma once

#include <string>
#include <type_traits>

namespace recon::moderncpp {

// if constexpr (C++17): branches evaluated and DISCARDED at compile
// time, based on a compile-time condition. The discarded branch is not
// merely skipped at runtime -- it is not even required to compile for
// the types where it isn't taken. That's what makes this work cleanly
// for fundamentally different types in one function template, without
// needing tag dispatch or SFINAE overload sets, the considerably
// clunkier techniques this replaced.
template <typename T>
std::string describeKind(const T& value) {
    if constexpr (std::is_pointer_v<T>) {
        // Only ever compiled for pointer types. Dereferencing `value`
        // here would be a compile error for a non-pointer T, but since
        // this whole branch is discarded for non-pointer T, it never
        // has to compile for those types in the first place.
        if (value == nullptr) {
            return "pointer (null)";
        }
        return "pointer (non-null)";
    } else if constexpr (std::is_arithmetic_v<T>) {
        return "arithmetic value";
    } else {
        return "some other type";
    }
}

}  // namespace recon::moderncpp
