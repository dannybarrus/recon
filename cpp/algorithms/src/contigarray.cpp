#include "recon/algorithms/contigarray.h"

namespace recon::algorithms {

namespace {
// Counts the longest run where each element equals the previous one
// plus `step` (step = 1 for ascending, step = -1 for descending).
//
// Note: the original relied on its own out-of-bounds read on the final
// iteration to "accidentally" flush the last run's length into the
// result -- the garbage value read past the array essentially never
// equaled the expected next value, so the comparison happened to fail,
// which happened to trigger the flush. That accidental side effect is
// made explicit and intentional here via the flush after the loop.
int countRunInDirection(const int* arr, int size, int step) {
    if (size <= 0) return 0;

    int maxRun = 1;
    int current = 1;

    for (int i = 0; i < size - 1; i++) {
        if (arr[i] + step == arr[i + 1]) {
            current++;
        } else {
            if (current > maxRun) maxRun = current;
            current = 1;
        }
    }
    if (current > maxRun) maxRun = current;  // flush the final run

    return maxRun;
}
}  // namespace

int countContiguousElements(const int* arr, int size) {
    int ascending = countRunInDirection(arr, size, 1);
    int descending = countRunInDirection(arr, size, -1);
    return ascending > descending ? ascending : descending;
}

}  // namespace recon::algorithms
