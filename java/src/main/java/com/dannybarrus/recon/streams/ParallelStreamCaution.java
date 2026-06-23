package com.dannybarrus.recon.streams;

import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.IntStream;

/**
 * Demonstrates the most common mistake with parallel streams -- mutating
 * shared state from inside one -- and the correct alternatives.
 *
 * <p>This is the same category of bug documented in this repo's Go bloom
 * filter project: multiple threads writing to the same memory without
 * synchronization. {@code parallelStream()} runs its operations across
 * multiple threads from the common {@code ForkJoinPool}, so anything that
 * was a single-threaded assumption in a sequential stream becomes a real
 * concurrency hazard the moment {@code parallel()} or {@code parallelStream()}
 * is introduced.
 *
 * <p>The broken version below is intentionally non-deterministic -- it
 * will not always produce a wrong answer on every run, which is exactly
 * what makes this class of bug dangerous. It can pass code review, pass in
 * dev, and only misbehave occasionally in production under load.
 *
 * <p><b>Note:</b> reproducing the race depends on genuine multi-core
 * parallelism with enough contention. On a machine with only one
 * available core (some constrained or virtualized environments), the
 * common {@code ForkJoinPool} has no real concurrency to exploit and
 * {@code countEvens_broken} may come back correct every time. That is not
 * the bug being fixed -- it's the absence of the conditions needed to
 * trigger it. The bug is still there in the code; it just needs real
 * parallel hardware and enough contention to show itself.
 */
public final class ParallelStreamCaution {

    private ParallelStreamCaution() { }

    /**
     * BROKEN: increments a plain {@code int} counter from multiple threads
     * with no synchronization. {@code count++} is not atomic -- it is
     * read-increment-write, and two threads can interleave those three
     * steps and lose an increment. Returns whatever total it happened to
     * land on, which may or may not equal {@code n} depending on timing.
     */
    public static int countEvens_broken(int n) {
        int[] count = {0};   // array used to allow mutation from inside the lambda
        IntStream.range(0, n)
            .parallel()
            .filter(i -> i % 2 == 0)
            .forEach(i -> count[0]++);   // DATA RACE -- not atomic
        return count[0];
    }

    /**
     * CORRECT, option 1: AtomicInteger. incrementAndGet() is a single
     * atomic hardware-backed operation, so concurrent increments from
     * multiple threads can never be lost.
     */
    public static int countEvens_atomicInteger(int n) {
        AtomicInteger count = new AtomicInteger(0);
        IntStream.range(0, n)
            .parallel()
            .filter(i -> i % 2 == 0)
            .forEach(i -> count.incrementAndGet());
        return count.get();
    }

    /**
     * CORRECT, option 2 (preferred): don't introduce mutable shared state
     * at all. {@code count()} is itself a terminal operation that the
     * stream implementation parallelizes safely internally -- there is
     * nothing for the caller to get wrong.
     */
    public static long countEvens_streamCount(int n) {
        return IntStream.range(0, n)
            .parallel()
            .filter(i -> i % 2 == 0)
            .count();
    }

    public static void main(String[] args) {
        int n = 1_000_000;

        System.out.println("--- Broken: shared mutable counter (run 3 times) ---");
        for (int i = 1; i <= 3; i++) {
            System.out.println("  Run " + i + ": " + countEvens_broken(n)
                + "   (expected: " + (n / 2) + " -- may or may not match)");
        }

        System.out.println("\n--- Correct: AtomicInteger ---");
        System.out.println("  " + countEvens_atomicInteger(n) + "   (expected: " + (n / 2) + ", always)");

        System.out.println("\n--- Correct (preferred): let the stream API do it ---");
        System.out.println("  " + countEvens_streamCount(n) + "   (expected: " + (n / 2) + ", always)");

        System.out.println("\nLesson: the fix is rarely \"add a lock.\" It's usually \"don't share");
        System.out.println("mutable state across threads in the first place.\" Prefer collectors");
        System.out.println("and built-in terminal operations over manual accumulation whenever a");
        System.out.println("stream might run in parallel.");
    }
}
