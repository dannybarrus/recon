#include "recon/algorithms/reversestring.h"

#include <cstddef>

namespace recon::algorithms {

char* reverseString(char* s) {
    char* p1 = s;
    char* p2 = s;

    while (*p2 != '\0') {
        p2++;
    }
    p2--;  // step back off the null terminator onto the last real character

    while (p1 < p2) {
        char temp = *p2;
        *p2 = *p1;
        *p1 = temp;
        p2--;
        p1++;
    }

    return s;
}

}  // namespace recon::algorithms
