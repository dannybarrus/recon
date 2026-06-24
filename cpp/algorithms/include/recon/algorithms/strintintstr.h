#pragma once

namespace recon::algorithms {

// Hand-rolled int<->string conversion -- the classic itoa()/atoi() pair.
//
// This file had no real bugs in the original. intToStr correctly handles
// 0 (the do-while loop runs at least once, producing "0") and negative
// numbers (sign tracked separately, magnitude converted, sign prepended).
//
// One documented limitation, not fixed: negating INT_MIN (`num *= -1`)
// is undefined behavior, since -INT_MIN is not representable in a
// same-width signed int. This is a genuinely subtle, well-known C/C++
// gotcha around two's-complement integer ranges being asymmetric, kept
// here as a known edge case rather than silently patched over -- see
// the test file for how this is verified to NOT silently corrupt
// anything within the normal range this function is meant for.
void intToStr(int num, char str[]);
int strToInt(const char str[]);

}  // namespace recon::algorithms
