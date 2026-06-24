#pragma once

namespace recon::algorithms {

// In-place string reversal via the classic two-pointer technique:
// one pointer starts at the front, one at the back, swap and walk
// inward until they meet. O(n) time, O(1) extra space.
//
// The algorithm itself was already correct in the original. The bug was
// in the caller: main() did `printf(szResult)` -- passing user/algorithm
// -derived data directly as printf's FORMAT STRING rather than as an
// argument. If the reversed string ever contained a '%' character, this
// is undefined behavior (classic CWE-134, format string vulnerability).
// It never manifested in the original because the test string happened
// to contain no '%' -- see the demo and tests for a case that would have
// exposed it.
char* reverseString(char* s);

}  // namespace recon::algorithms
