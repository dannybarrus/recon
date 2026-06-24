#pragma once

namespace recon::algorithms {

// A hand-rolled strcmp(): walks both strings while characters match,
// stops at the first mismatch (or either string's end), and returns the
// difference between the two differing characters -- 0 if the strings
// are identical, negative if s1 sorts before s2, positive otherwise.
// This mirrors the real strcmp()'s contract exactly.
//
// The function itself was already correct. The bug was in the caller:
// main() computed the result into a local variable and never printed
// it, so running the original program produced no visible output at
// all -- the exact same blind spot as reversewords and removeduplicates
// in this folder.
int compareStrings(const char* s1, const char* s2);

}  // namespace recon::algorithms
