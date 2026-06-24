#include "recon/algorithms/strintintstr.h"

namespace recon::algorithms {

void intToStr(int num, char str[]) {
    constexpr int kMaxDigitsInt = 10;
    char temp[kMaxDigitsInt + 2];

    int i = 0;
    int j = 0;
    bool isNeg = false;

    if (num < 0) {
        num *= -1;  // NOTE: undefined behavior if num == INT_MIN -- documented in the header
        isNeg = true;
    }

    do {
        temp[i++] = static_cast<char>((num % 10) + '0');
        num /= 10;
    } while (num);

    if (isNeg) {
        str[j++] = '-';
    }

    while (i > 0) {
        str[j++] = temp[--i];
    }

    str[j] = '\0';
}

int strToInt(const char str[]) {
    int i = 0;
    bool isNeg = false;
    int num = 0;

    if (str[0] == '-') {
        isNeg = true;
        i++;
    }

    while (str[i]) {
        num *= 10;
        num += (str[i++] - '0');
    }

    if (isNeg) {
        num *= -1;
    }

    return num;
}

}  // namespace recon::algorithms
