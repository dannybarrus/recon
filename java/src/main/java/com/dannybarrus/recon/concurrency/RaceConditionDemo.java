package com.dannybarrus.recon.concurrency;

import java.util.concurrent.atomic.AtomicInteger;

/**
 * Demonstrates the lost-update race condition on a plain {@code int}
 * field, and two different correct fixes: {@code synchronized} and
 * {@code AtomicInteger}.
 *
 * <p>Note: like {@code ParallelStreamCaution} in the streams package,
 * reproducing the race depends on genuine multi-core parallelism and
 * enough contention. On a single-core or lightly-loaded machine, the
 * broken version may occasionally come back correct anyway -- that does
 * not mean the bug isn't there, only that nothing happened to expose it
 * on that particular run.
 */
public final class RaceConditionDemo {

    private RaceConditionDemo() { }

    private static final int THREAD_COUNT = 10;
    private static final int INCREMENTS_PER_THREAD = 100_000;
    private static final int EXPECTED = THREAD_COUNT * INCREMENTS_PER_THREAD;

    public static void main(String[] args) throws InterruptedException {
        demoUnsafeCounter();
        System.out.println();
        demoSafeCounter();
        System.out.println();
        demoAtomicInteger();
    }

    private static void demoUnsafeCounter() throws InterruptedException {
        System.out.println("--- Broken: UnsafeCounter (run 3 times) ---");
        for (int run = 1; run <= 3; run++) {
            UnsafeCounter counter = new UnsafeCounter();
            runConcurrently(counter::increment);
            System.out.println("  Run " + run + ": " + counter.get()
                + "   (expected: " + EXPECTED + " -- may or may not match)");
        }
    }

    private static void demoSafeCounter() throws InterruptedException {
        System.out.println("--- Correct: SafeCounter (synchronized) ---");
        SafeCounter counter = new SafeCounter();
        runConcurrently(counter::increment);
        System.out.println("  " + counter.get() + "   (expected: " + EXPECTED + ", always)");
    }

    private static void demoAtomicInteger() throws InterruptedException {
        System.out.println("--- Correct: AtomicInteger ---");
        AtomicInteger counter = new AtomicInteger(0);
        runConcurrently(counter::incrementAndGet);
        System.out.println("  " + counter.get() + "   (expected: " + EXPECTED + ", always)");
    }

    private static void runConcurrently(Runnable incrementTask) throws InterruptedException {
        Thread[] threads = new Thread[THREAD_COUNT];
        for (int i = 0; i < THREAD_COUNT; i++) {
            threads[i] = new Thread(() -> {
                for (int j = 0; j < INCREMENTS_PER_THREAD; j++) {
                    incrementTask.run();
                }
            });
        }
        for (Thread t : threads) t.start();
        for (Thread t : threads) t.join();
    }
}
