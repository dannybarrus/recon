#include "recon/algorithms/bitcounting.h"

#include <climits>
#include <cstdio>

namespace recon::algorithms {

int iteratedBitcount(unsigned int n) {
    int count = 0;
    while (n) {
        count += n & 0x1u;
        n >>= 1;
    }
    return count;
}

int sparseOnesBitcount(unsigned int n) {
    int count = 0;
    while (n) {
        count++;
        n &= (n - 1);  // clears the lowest set bit
    }
    return count;
}

int denseOnesBitcount(unsigned int n) {
    int count = 8 * static_cast<int>(sizeof(int));  // assumes 32-bit int
    n ^= static_cast<unsigned int>(-1);              // invert: count zeros instead
    while (n) {
        count--;
        n &= (n - 1);
    }
    return count;
}

namespace {
// File-scope lookup tables -- populated once by computeBitcountTables().
int bitsInChar[256];
char bitsIn16Bits[0x1u << 16];
}  // namespace

void computeBitcountTables() {
    for (unsigned int i = 0; i < 256; i++) {
        bitsInChar[i] = iteratedBitcount(i);
    }
    for (unsigned int i = 0; i < (0x1u << 16); i++) {
        bitsIn16Bits[i] = static_cast<char>(iteratedBitcount(i));
    }
}

int precomputedBitcount(unsigned int n) {
    // works only for 32-bit ints
    return bitsInChar[n & 0xffu]
         + bitsInChar[(n >> 8) & 0xffu]
         + bitsInChar[(n >> 16) & 0xffu]
         + bitsInChar[(n >> 24) & 0xffu];
}

int precomputed16Bitcount(unsigned int n) {
    // works only for 32-bit ints
    return bitsIn16Bits[n & 0xffffu]
         + bitsIn16Bits[(n >> 16) & 0xffffu];
}

// Bit-parallel divide and conquer: after each COUNT() step, groups of bits
// twice as wide each hold the running popcount for that group.
#define TWO(c) (0x1u << (c))
#define MASK(c) (((unsigned int)(-1)) / (TWO(TWO(c)) + 1u))
#define COUNT(x, c) (((x) & MASK(c)) + (((x) >> (TWO(c))) & MASK(c)))

int parallelBitcount(unsigned int n) {
    n = COUNT(n, 0);
    n = COUNT(n, 1);
    n = COUNT(n, 2);
    n = COUNT(n, 3);
    n = COUNT(n, 4);
    return static_cast<int>(n);
}

#define MASK_01010101 (((unsigned int)(-1)) / 3)
#define MASK_00110011 (((unsigned int)(-1)) / 5)
#define MASK_00001111 (((unsigned int)(-1)) / 17)

int niftyBitcount(unsigned int n) {
    n = (n & MASK_01010101) + ((n >> 1) & MASK_01010101);
    n = (n & MASK_00110011) + ((n >> 2) & MASK_00110011);
    n = (n & MASK_00001111) + ((n >> 4) & MASK_00001111);
    return static_cast<int>(n % 255);
}

// HACKMEM 169 (MIT AI Lab memo, late 1970s), as used in X11 sources.
// Sums octal digits via modulo-63 ("casting out nines" generalized to base 64).
int mitBitcount(unsigned int n) {
    unsigned int tmp = n - ((n >> 1) & 033333333333u) - ((n >> 2) & 011111111111u);
    return static_cast<int>(((tmp + (tmp >> 3)) & 030707070707u) % 63);
}

bool verifyBitcounts(unsigned int x) {
    int iterated = iteratedBitcount(x);
    int sparse = sparseOnesBitcount(x);
    int dense = denseOnesBitcount(x);
    int precomputed = precomputedBitcount(x);
    int precomputed16 = precomputed16Bitcount(x);
    int parallel = parallelBitcount(x);
    int nifty = niftyBitcount(x);
    int mit = mitBitcount(x);

    // The original called exit(0) on any mismatch -- inside a reusable
    // helper function, that is a serious anti-pattern: it makes the
    // function impossible to unit test (a failing case would kill the
    // entire test process rather than reporting a failed assertion) and
    // impossible to call safely from any larger program. Returning a
    // bool and letting the caller decide what to do is the fix.
    if (iterated != sparse) { std::printf("MISMATCH: sparseOnesBitcount(0x%x)\n", x); return false; }
    if (iterated != dense) { std::printf("MISMATCH: denseOnesBitcount(0x%x)\n", x); return false; }
    if (iterated != precomputed) { std::printf("MISMATCH: precomputedBitcount(0x%x)\n", x); return false; }
    if (iterated != precomputed16) { std::printf("MISMATCH: precomputed16Bitcount(0x%x)\n", x); return false; }
    if (iterated != parallel) { std::printf("MISMATCH: parallelBitcount(0x%x)\n", x); return false; }
    if (iterated != nifty) { std::printf("MISMATCH: niftyBitcount(0x%x)\n", x); return false; }
    if (mit != nifty) { std::printf("MISMATCH: mitBitcount(0x%x)\n", x); return false; }

    return true;
}

}  // namespace recon::algorithms
