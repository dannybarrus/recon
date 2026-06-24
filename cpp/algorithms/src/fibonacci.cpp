#include "recon/algorithms/fibonacci.h"

namespace recon::algorithms {

int fibonacciNaive(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fibonacciNaive(n - 1) + fibonacciNaive(n - 2);
}

int fibonacciIterative(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;

    int prev = 0;
    int curr = 1;
    for (int i = 2; i <= n; i++) {
        int next = prev + curr;
        prev = curr;
        curr = next;
    }
    return curr;
}

}  // namespace recon::algorithms
