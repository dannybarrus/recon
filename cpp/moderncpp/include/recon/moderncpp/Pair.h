#pragma once

#include <utility>

namespace recon::moderncpp {

// A minimal class template -- two type parameters, working for any
// combination of types. This is the standard library's std::pair,
// reimplemented to make the template mechanism itself visible rather
// than reaching for the pre-built version immediately, the same
// approach taken with ScopedLock in the memory module (which
// reimplements std::lock_guard for the same reason).
//
// Note that the ENTIRE class lives in this header. Class templates are
// not compiled into object code until they're actually instantiated
// with a concrete set of types -- the compiler needs the full
// definition visible at every call site to generate the right code for
// whichever T1/T2 combination is actually used. This is why templates
// are conventionally header-only, unlike the rest of this codebase's
// .h/.cpp split.
template <typename T1, typename T2>
class Pair {
public:
    Pair(T1 first, T2 second) : first_(std::move(first)), second_(std::move(second)) {}

    const T1& first() const { return first_; }
    const T2& second() const { return second_; }

    void swap(Pair& other) {
        std::swap(first_, other.first_);
        std::swap(second_, other.second_);
    }

    bool operator==(const Pair& other) const {
        return first_ == other.first_ && second_ == other.second_;
    }

private:
    T1 first_;
    T2 second_;
};

// A function template that lets the compiler deduce T1 and T2 from the
// arguments, so callers can write makePair(1, "two") instead of
// Pair<int, const char*>(1, "two"). std::make_pair does exactly this
// for the standard library's own pair.
template <typename T1, typename T2>
Pair<T1, T2> makePair(T1 first, T2 second) {
    return Pair<T1, T2>(std::move(first), std::move(second));
}

}  // namespace recon::moderncpp
