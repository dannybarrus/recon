#include "recon/algorithms/removeduplicates.h"

namespace recon::algorithms {

int removeDuplicates(int* arr, int size) {
    if (size == 0) return 0;

    int current = 1;
    int insert = 1;

    // Note: simplified from the original's if/else (both branches
    // incremented `current` anyway -- consolidated to one increment
    // per loop pass). Same algorithm, same result, less duplication.
    while (current < size) {
        if (arr[current] != arr[insert - 1]) {
            arr[insert] = arr[current];
            insert++;
        }
        current++;
    }

    return insert;
}

}  // namespace recon::algorithms
