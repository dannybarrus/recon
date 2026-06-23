package com.dannybarrus.recon.concurrency;

/**
 * Demonstrates two SEPARATE things about {@code volatile}, often confused
 * for one another:
 *
 * <ol>
 *   <li><b>Visibility:</b> a write to a {@code volatile} field by one
 *       thread is guaranteed to be visible to other threads that read it
 *       afterward. Without {@code volatile}, the JIT compiler and CPU
 *       caching are legally permitted to let one thread keep reading a
 *       stale cached value indefinitely, never noticing another thread
 *       changed it.</li>
 *   <li><b>NOT atomicity:</b> {@code volatile} does NOT make compound
 *       operations (read-modify-write, like {@code count++}) atomic. A
 *       volatile {@code int} can still lose updates under concurrent
 *       increments, for exactly the same reason a plain {@code int} does
 *       -- {@code ++} is three separate steps, and {@code volatile} only
 *       guarantees each individual read or write is visible, not that the
 *       whole sequence happens without another thread interleaving in the
 *       middle.</li>
 * </ol>
 *
 * <p><b>Note on reproducibility:</b> the visibility demonstration below
 * depends on the JIT actually caching a read in a tight loop, which modern
 * JVMs do not guarantee to do within any fixed, short time window -- it is
 * a legal optimization, not a forced one. This demo bounds the wait with a
 * timeout specifically so it can never hang, and reports honestly whether
 * stale-read behaviour was actually observed on this run. The atomicity
 * demonstration in the second half has no such caveat -- lost updates
 * under concurrent increments are reliably reproducible with enough
 * contention on any genuinely multi-core machine.
 */
public final class VolatileVisibilityDemo {

    private VolatileVisibilityDemo() { }

    private static boolean nonVolatileFlag = false;
    private static volatile boolean volatileFlag = false;

    public static void main(String[] args) throws InterruptedException {
        demoVisibility();
        System.out.println();
        demoAtomicityIsNotGuaranteedByVolatileAlone();
    }

    private static void demoVisibility() throws InterruptedException {
        System.out.println("--- Visibility: does the worker thread ever see the flag flip? ---");

        System.out.println("  Using a NON-VOLATILE flag:");
        runVisibilityTrial(false);

        System.out.println("  Using a VOLATILE flag:");
        runVisibilityTrial(true);
    }

    private static void runVisibilityTrial(boolean useVolatile) throws InterruptedException {
        nonVolatileFlag = false;
        volatileFlag = false;

        Thread worker = new Thread(() -> {
            if (useVolatile) {
                while (!volatileFlag) {
                    // busy-spin
                }
            } else {
                while (!nonVolatileFlag) {
                    // busy-spin
                }
            }
        });
        worker.setDaemon(true);
        worker.start();

        Thread.sleep(50);

        if (useVolatile) {
            volatileFlag = true;
        } else {
            nonVolatileFlag = true;
        }

        worker.join(1000);

        if (worker.isAlive()) {
            System.out.println("    Worker did NOT stop within 1 second -- it may still be");
            System.out.println("    spinning on a stale cached value. (Daemon thread -- safe to abandon.)");
        } else {
            System.out.println("    Worker stopped promptly after the flag was set.");
        }
    }

    private static void demoAtomicityIsNotGuaranteedByVolatileAlone() throws InterruptedException {
        System.out.println("--- volatile does NOT make count++ atomic ---");

        VolatileCounter counter = new VolatileCounter();
        int threadCount = 10;
        int incrementsPerThread = 100_000;
        int expected = threadCount * incrementsPerThread;

        Thread[] threads = new Thread[threadCount];
        for (int i = 0; i < threadCount; i++) {
            threads[i] = new Thread(() -> {
                for (int j = 0; j < incrementsPerThread; j++) {
                    counter.incrementUnsafely();
                }
            });
        }
        for (Thread t : threads) t.start();
        for (Thread t : threads) t.join();

        System.out.println("  Final count: " + counter.get()
            + "   (expected: " + expected + " -- volatile alone usually does NOT achieve this)");
        System.out.println("  volatile guarantees each read/write is visible -- it does not");
        System.out.println("  guarantee the read-increment-write sequence happens without");
        System.out.println("  another thread interleaving in the middle of it.");
    }

    private static class VolatileCounter {
        private volatile int count = 0;

        void incrementUnsafely() {
            count++;
        }

        int get() {
            return count;
        }
    }
}
