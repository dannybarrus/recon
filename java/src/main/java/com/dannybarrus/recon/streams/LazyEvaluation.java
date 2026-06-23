package com.dannybarrus.recon.streams;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * Demonstrates two properties of streams that are easy to state but easy
 * to get wrong intuitively until you've watched them happen:
 *
 * <ol>
 *   <li><b>Laziness:</b> intermediate operations ({@code filter}, {@code map},
 *       etc.) do not run when you call them. They only run when a terminal
 *       operation ({@code collect}, {@code forEach}, {@code findFirst}, ...)
 *       pulls elements through the pipeline.</li>
 *   <li><b>Per-element pipelining:</b> a common misconception is that a
 *       chain like {@code .filter(...).map(...)} runs {@code filter} across
 *       the <em>entire</em> input first, then runs {@code map} across the
 *       entire filtered result. It does not. Each element is pushed through
 *       the <em>whole</em> pipeline -- filter, then map -- before the next
 *       element is even looked at.</li>
 * </ol>
 *
 * <p>Each method here records a string into the supplied {@code trace} list
 * every time an intermediate operation actually executes against an
 * element. The trace is what makes these properties verifiable in a unit
 * test, rather than something you just have to take on faith from a
 * println-based demo.
 */
public final class LazyEvaluation {

    private LazyEvaluation() { }

    /**
     * Builds a filter+map pipeline and records every step. Proves both
     * laziness (nothing in {@code trace} until the terminal op runs) and
     * per-element pipelining (filter and map interleave per element,
     * rather than running as two separate full passes).
     */
    public static List<Integer> buildAndRunPipeline(List<Integer> input, List<String> trace) {
        trace.add("pipeline built");

        Stream<Integer> pipeline = input.stream()
            .filter(n -> { trace.add("filter(" + n + ")"); return n % 2 == 0; })
            .map(n -> { trace.add("map(" + n + ")"); return n * 10; });

        trace.add("about to call terminal operation");
        List<Integer> result = pipeline.collect(Collectors.toList());
        trace.add("terminal operation finished");
        return result;
    }

    /**
     * Demonstrates short-circuiting: {@code findFirst()} stops pulling
     * elements through the pipeline as soon as it finds one match, even if
     * the input has many more elements after it.
     */
    public static Integer findFirstEvenGreaterThan(List<Integer> input, int threshold, List<String> trace) {
        return input.stream()
            .filter(n -> { trace.add("checked(" + n + ")"); return n > threshold && n % 2 == 0; })
            .findFirst()
            .orElse(null);
    }

    /**
     * Stream.iterate + limit -- an infinite logical sequence, made finite
     * by limit(). The generator function is only ever called as many times
     * as limit() allows; it does not (and could not) run to completion on
     * its own, since there is no natural end to "double the previous value."
     */
    public static List<Long> firstNPowersOfTwo(int n) {
        return Stream.iterate(1L, x -> x * 2)
            .limit(n)
            .collect(Collectors.toList());
    }

    public static void main(String[] args) {
        System.out.println("--- Laziness and per-element pipelining ---");
        List<String> trace = new ArrayList<>();
        List<Integer> result = buildAndRunPipeline(List.of(1, 2, 3, 4), trace);
        trace.forEach(line -> System.out.println("  " + line));
        System.out.println("Result: " + result);

        System.out.println("\n--- Short-circuiting with findFirst() ---");
        List<String> shortCircuitTrace = new ArrayList<>();
        Integer found = findFirstEvenGreaterThan(List.of(1, 2, 3, 4, 5, 6, 7, 8, 9, 10), 4, shortCircuitTrace);
        shortCircuitTrace.forEach(line -> System.out.println("  " + line));
        System.out.println("Found: " + found + "   (checked only "
            + shortCircuitTrace.size() + " of 10 elements before stopping)");

        System.out.println("\n--- Infinite sequence, made finite with limit() ---");
        System.out.println("First 10 powers of two: " + firstNPowersOfTwo(10));
    }
}
