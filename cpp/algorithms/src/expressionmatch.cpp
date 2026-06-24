#include "recon/algorithms/expressionmatch.h"

namespace recon::algorithms {

bool matchesPattern(const char* text, const char* pattern) {
    // Base case: pattern exhausted. Match only if text is also exhausted.
    if (*pattern == '\0') {
        return *text == '\0';
    }

    if (*pattern == '*') {
        // Try matching the rest of the pattern against every possible
        // remaining suffix of text -- '*' consuming zero characters,
        // one character, two characters, and so on, until text runs out.
        // This is what the original never actually did for anything
        // after the FIRST wildcard.
        const char* t = text;
        do {
            if (matchesPattern(t, pattern + 1)) {
                return true;
            }
        } while (*t++ != '\0');
        return false;
    }

    // Literal character: must match exactly, and text must not already
    // be exhausted.
    if (*text != '\0' && *text == *pattern) {
        return matchesPattern(text + 1, pattern + 1);
    }

    return false;
}

}  // namespace recon::algorithms
