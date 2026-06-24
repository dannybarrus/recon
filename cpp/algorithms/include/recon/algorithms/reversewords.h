#pragma once

namespace recon::algorithms {

// Reverses the order of words in a sentence in place, in O(n) time with
// O(1) extra space, using the classic "reverse the whole string, then
// reverse each word back" trick:
//   "This is a test"
//   -> reverse whole:   "tset a si sihT"
//   -> reverse each word in place (word boundaries unchanged):
//                        "test a is This"
//
// The algorithm was already correct in the original. The bug was in the
// caller: main() called ReverseWords() and never printed the result --
// running the original program produced no visible output at all, so
// there was no way to actually see the algorithm work. Two other files
// in this folder (comparestr, removeduplicates) had the exact same
// blind spot -- a correct algorithm with no visible proof it ran.
void reverseWords(char str[]);

}  // namespace recon::algorithms
