package com.dannybarrus.recon.streams;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

/**
 * Tests the two CORRECT implementations only. The broken version is
 * intentionally not asserted on here -- asserting a race condition
 * "fails" reliably would itself be a flaky, environment-dependent test,
 * which is exactly the anti-pattern called out elsewhere in this repo
 * (see CollectionPerformance). The broken version is demonstrated in
 * main(), not tested for failure here.
 */
class ParallelStreamCautionTest {

    @Test
    void countEvens_atomicInteger_isAlwaysCorrect() {
        assertEquals(50, ParallelStreamCaution.countEvens_atomicInteger(100));
    }

    @Test
    void countEvens_atomicInteger_handlesOddTotal() {
        // 0..100 inclusive-exclusive range of size 101 -> evens are 0,2,4,...,100 = 51
        assertEquals(51, ParallelStreamCaution.countEvens_atomicInteger(101));
    }

    @Test
    void countEvens_streamCount_isAlwaysCorrect() {
        assertEquals(50, ParallelStreamCaution.countEvens_streamCount(100));
    }

    @Test
    void bothCorrectImplementations_agreeWithEachOther() {
        int n = 10_000;
        assertEquals(
            ParallelStreamCaution.countEvens_atomicInteger(n),
            (int) ParallelStreamCaution.countEvens_streamCount(n));
    }

    @Test
    void countEvens_zeroElements_returnsZero() {
        assertEquals(0, ParallelStreamCaution.countEvens_atomicInteger(0));
        assertEquals(0L, ParallelStreamCaution.countEvens_streamCount(0));
    }
}
