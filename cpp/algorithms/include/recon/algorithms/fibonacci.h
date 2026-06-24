#pragma once

namespace recon::algorithms {

// The original was a correct but naive recursive Fibonacci -- O(2^n) time,
// recomputing the same sub-values over and over. Kept as-is below
// (renamed fibonacciNaive) specifically so it can be timed against the
// iterative version, the same "naive vs optimized" comparison already
// done for Fibonacci in the C# and Go parts of this repo.

int fibonacciNaive(int n);     // O(2^n) time, O(n) stack depth
int fibonacciIterative(int n); // O(n) time, O(1) space

}  // namespace recon::algorithms
