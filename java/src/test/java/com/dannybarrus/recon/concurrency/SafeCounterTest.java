package com.dannybarrus.recon.concurrency;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

/**
 * Tests the CORRECT implementation only -- see {@code ParallelStreamCautionTest}
 * in the streams package for why asserting on the broken {@code UnsafeCounter}
 * would itself be a flaky-test anti-pattern.
 */
class SafeCounterTest {

    @Test
    void increment_singleThreaded_countsCorrectly() {
        SafeCounter counter = new SafeCounter();
        for (int i = 0; i < 100; i++) {
            counter.increment();
        }
        assertEquals(100, counter.get());
    }

    @Test
    void increment_concurrent_neverLosesAnUpdate() throws InterruptedException {
        SafeCounter counter = new SafeCounter();
        int threadCount = 10;
        int incrementsPerThread = 10_000;

        Thread[] threads = new Thread[threadCount];
        for (int i = 0; i < threadCount; i++) {
            threads[i] = new Thread(() -> {
                for (int j = 0; j < incrementsPerThread; j++) {
                    counter.increment();
                }
            });
        }
        for (Thread t : threads) t.start();
        for (Thread t : threads) t.join();

        assertEquals(threadCount * incrementsPerThread, counter.get());
    }
}
