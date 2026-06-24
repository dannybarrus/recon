#pragma once

namespace recon::algorithms {

// Removes duplicates from a SORTED array in place, using the classic
// two-pointer technique: `current` scans forward through the input,
// `insert` marks the next write position for a value not yet seen.
// O(n) time, O(1) extra space.
//
// Returns the new logical length of the array (the number of unique
// elements). Elements at indices >= the returned length are leftover
// garbage from the original array and should be ignored by the caller.
//
// The algorithm was already correct. The bug was in the caller: main()
// computed the de-duplicated result into a second array via memcpy and
// then never printed it -- same "silent main()" blind spot as
// reversewords and comparestr in this same folder.
int removeDuplicates(int* arr, int size);

}  // namespace recon::algorithms
