#pragma once

namespace recon::algorithms {

// Several different ways to count the number of set bits (1s) in an
// unsigned 32-bit integer -- "population count" / "popcount". This is
// genuinely solid material as it stood originally: well-commented,
// each technique correct, and self-verifying (verify_bitcounts checks
// every implementation agrees with every other for a given input).
//
// All eight techniques are kept here because each demonstrates a
// different idea worth being able to explain:
//   - iterated:      the obvious O(bits) loop, shift-and-mask
//   - sparseOnes:    O(number of 1 bits) -- n &= (n-1) clears the
//                    lowest set bit each iteration
//   - denseOnes:     O(number of 0 bits) -- invert n, then run the
//                    sparse-ones trick on the zeros instead
//   - precomputed:   O(1) via a 256-entry lookup table, one lookup
//                    per byte of the input
//   - precomputed16: O(1) via a 65536-entry lookup table, one lookup
//                    per 16-bit half of the input -- fewer lookups,
//                    much bigger table; the classic time/space tradeoff
//   - parallel:      bit-parallel divide and conquer -- sum adjacent
//                    bit pairs, then nibbles, then bytes, then halves
//   - nifty:         the same idea as parallel but stops after summing
//                     into bytes, then uses (mod 255) to finish the sum
//                     in one division instead of further bit-shifting
//   - mit:           HACKMEM 169 -- base-8 digit-sum trick via modulo
//                     arithmetic ("casting out nines", but base 63)

int iteratedBitcount(unsigned int n);
int sparseOnesBitcount(unsigned int n);
int denseOnesBitcount(unsigned int n);

// Must be called once before using precomputedBitcount / precomputed16Bitcount.
void computeBitcountTables();

int precomputedBitcount(unsigned int n);
int precomputed16Bitcount(unsigned int n);

int parallelBitcount(unsigned int n);
int niftyBitcount(unsigned int n);
int mitBitcount(unsigned int n);

// Verifies all eight implementations agree on the given input.
// Returns true if they all match, false (and prints which one disagreed)
// if any implementation diverges.
bool verifyBitcounts(unsigned int x);

}  // namespace recon::algorithms
