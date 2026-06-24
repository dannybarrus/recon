#include "recon/algorithms/reversewords.h"

#include <cstring>

namespace recon::algorithms {

namespace {
void reverseRange(char str[], int start, int end) {
    while (end > start) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
}  // namespace

void reverseWords(char str[]) {
    int length = static_cast<int>(std::strlen(str));

    reverseRange(str, 0, length - 1);

    int end = 0;
    while (end < length) {
        if (str[end] != ' ') {
            int start = end;
            while (end < length && str[end] != ' ') {
                end++;
            }
            end--;
            reverseRange(str, start, end);
        }
        end++;
    }
}

}  // namespace recon::algorithms
