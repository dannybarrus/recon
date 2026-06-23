package com.dannybarrus.recon.collections;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

/**
 * Empirically demonstrates the performance difference between
 * {@code ArrayList} and {@code LinkedList} for common operations.
 *
 * <p>Theoretical Big-O is necessary but not sufficient for an interview
 * answer -- this class actually times the operations so the difference is
 * concrete rather than asserted.
 *
 * <table border="1">
 *   <caption>Time complexity summary</caption>
 *   <tr><th>Operation</th><th>ArrayList</th><th>LinkedList</th></tr>
 *   <tr><td>add at end</td><td>O(1) amortized</td><td>O(1)</td></tr>
 *   <tr><td>add at front</td><td>O(n) -- shifts every element</td><td>O(1)</td></tr>
 *   <tr><td>get(index)</td><td>O(1) -- backed by an array</td><td>O(n) -- must walk the chain</td></tr>
 *   <tr><td>contains / indexOf</td><td>O(n)</td><td>O(n)</td></tr>
 * </table>
 *
 * <p><b>Note on testing this class:</b> the test suite for this class
 * verifies <em>correctness</em> (operations produce the right results) but
 * does not assert on timing numbers. Asserting exact durations in a unit
 * test is a well-known anti-pattern -- timing varies by machine, JIT
 * warmup, and system load, and would make the test suite flaky. The timing
 * output here is for human-readable demonstration only.
 */
public final class CollectionPerformance {

    private CollectionPerformance() { }

    /** Appends n integers (0..n-1) to the end of the given list, in order. */
    public static void appendN(List<Integer> list, int n) {
        for (int i = 0; i < n; i++) {
            list.add(i);
        }
    }

    /** Inserts n integers at index 0, one at a time -- worst case for ArrayList. */
    public static void prependN(List<Integer> list, int n) {
        for (int i = 0; i < n; i++) {
            list.add(0, i);
        }
    }

    /** Reads every element by index, front to back -- worst case for LinkedList. */
    public static long sumByIndex(List<Integer> list) {
        long sum = 0;
        for (int i = 0; i < list.size(); i++) {
            sum += list.get(i);
        }
        return sum;
    }

    public static void main(String[] args) {
        final int n = 20_000;

        System.out.println("--- Append " + n + " elements at the end ---");
        timeOperation("ArrayList",  () -> appendN(new ArrayList<>(), n));
        timeOperation("LinkedList", () -> appendN(new LinkedList<>(), n));

        System.out.println("\n--- Prepend " + n + " elements at index 0 ---");
        timeOperation("ArrayList",  () -> prependN(new ArrayList<>(), n));
        timeOperation("LinkedList", () -> prependN(new LinkedList<>(), n));

        System.out.println("\n--- Read all elements by index (after appending " + n + ") ---");
        List<Integer> arrayList = new ArrayList<>();
        appendN(arrayList, n);
        List<Integer> linkedList = new LinkedList<>();
        appendN(linkedList, n);

        timeOperation("ArrayList",  () -> sumByIndex(arrayList));
        timeOperation("LinkedList", () -> sumByIndex(linkedList));

        System.out.println("\nConclusion: ArrayList wins for indexed reads and end-appends.");
        System.out.println("LinkedList wins for front-insertion. Neither wins at everything --");
        System.out.println("choose based on the operation your code does most often.");
    }

    private static void timeOperation(String label, Runnable operation) {
        long start = System.nanoTime();
        operation.run();
        long elapsedMs = (System.nanoTime() - start) / 1_000_000;
        System.out.printf("  %-12s %4d ms%n", label, elapsedMs);
    }

    @FunctionalInterface
    private interface LongSupplier {
        long get();
    }

    private static void timeOperation(String label, LongSupplier operation) {
        long start = System.nanoTime();
        operation.get();
        long elapsedMs = (System.nanoTime() - start) / 1_000_000;
        System.out.printf("  %-12s %4d ms%n", label, elapsedMs);
    }
}
