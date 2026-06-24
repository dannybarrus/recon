#include "recon/algorithms/custom_strchr.h"

namespace recon::algorithms {

const char* customStrchr(const char* s, int c) {
    // Fixed: check *s BEFORE advancing, not after. The original's
    // `while (*s++) { if (*s == c) ... }` always tested the position
    // one past wherever the loop condition had just looked, permanently
    // skipping index 0.
    while (*s != '\0') {
        if (*s == c) {
            return s;
        }
        s++;
    }
    // Standard strchr() also matches the null terminator itself when
    // c == '\0' -- honor that here too, rather than only handling the
    // non-null-character case.
    if (c == '\0') {
        return s;
    }
    return nullptr;
}

}  // namespace recon::algorithms
