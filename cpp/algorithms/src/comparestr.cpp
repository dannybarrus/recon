#include "recon/algorithms/comparestr.h"

namespace recon::algorithms {

int compareStrings(const char* s1, const char* s2) {
    // Note: the original wrote `*s1++;` and `*s2++;` as bare statements.
    // Postfix ++ binds tighter than unary *, so this parses as
    // `*(s1++)` -- it dereferences the OLD pointer value (discarding the
    // result, a wasted read) and then advances the pointer. Functionally
    // equivalent to a plain `s1++;`, just confusingly written. Simplified
    // here to what it actually means.
    while ((*s1 != '\0') && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *s1 - *s2;
}

}  // namespace recon::algorithms
