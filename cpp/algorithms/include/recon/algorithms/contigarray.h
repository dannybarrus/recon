#pragma once

namespace recon::algorithms {

// Finds the length of the longest run of contiguous elements in an array
// -- a run where each element is exactly one more (ascending) or one
// less (descending) than the previous element. Checks both directions
// and returns whichever run is longer.
//
// BUG FOUND AND FIXED (confirmed with AddressSanitizer -- a real
// stack-buffer-overflow, not just a theoretical concern): the original
// read `p[nIndex + 1]` on every iteration including when nIndex was the
// last valid index, reading one element past the end of the array.
// AddressSanitizer aborted the program outright on this:
//
//   ERROR: AddressSanitizer: stack-buffer-overflow ...
//   READ of size 4 ... <== Memory access at offset 96 overflows this
//   variable
//
// Fixed by stopping the comparison one element earlier (loop bound
// `size - 1` instead of `size`), since a run-check inherently needs a
// next element to compare against.
int countContiguousElements(const int* arr, int size);

}  // namespace recon::algorithms
