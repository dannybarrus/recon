package com.dannybarrus.recon.streams;

import java.util.List;

/**
 * Demonstrates {@code reduce()} -- the general-purpose "fold" operation
 * that most {@code Collectors} are themselves built on top of.
 *
 * <p>Two forms are used here:
 * <ul>
 *   <li><b>Two-arg reduce</b> {@code (identity, accumulator)} -- used for
 *       sequential streams. Identity is the starting value and the result
 *       if the stream is empty.</li>
 *   <li><b>Three-arg reduce</b> {@code (identity, accumulator, combiner)}
 *       -- required for parallel streams, where the stream is split into
 *       chunks, each chunk is reduced independently using the accumulator,
 *       and the combiner merges the partial results back together.</li>
 * </ul>
 *
 * <p>The point of building these by hand is to make the relationship
 * explicit: {@code Collectors.summingInt(...)} and a manual
 * {@code reduce(0, Integer::sum)} compute the same answer, because the
 * built-in collector is doing exactly this under the hood.
 */
public final class ReduceExamples {

    private ReduceExamples() { }

    /** Sum via reduce -- identity 0, accumulator adds. */
    public static int sumWithReduce(List<Integer> numbers) {
        return numbers.stream().reduce(0, Integer::sum);
    }

    /** Product via reduce -- identity 1 (the multiplicative identity), accumulator multiplies. */
    public static int productWithReduce(List<Integer> numbers) {
        return numbers.stream().reduce(1, (a, b) -> a * b);
    }

    /** Max via reduce -- no natural "identity" for max, so the no-identity overload returns Optional. */
    public static int maxWithReduce(List<Integer> numbers) {
        return numbers.stream()
            .reduce(Integer::max)
            .orElseThrow(() -> new IllegalArgumentException("Cannot find max of an empty list"));
    }

    /**
     * Sum via the three-arg reduce, safe for parallel execution.
     * identity=0, accumulator adds an int to the running total,
     * combiner merges two partial totals from different chunks.
     */
    public static int sumWithParallelReduce(List<Integer> numbers) {
        return numbers.parallelStream()
            .reduce(0,
                (partialSum, next) -> partialSum + next,   // accumulator
                Integer::sum);                              // combiner
    }

    /** String concatenation via reduce, demonstrating reduce is not limited to numbers. */
    public static String concatenateWithReduce(List<String> words) {
        return words.stream().reduce("", (a, b) -> a.isEmpty() ? b : a + " " + b);
    }

    public static void main(String[] args) {
        List<Integer> numbers = List.of(1, 2, 3, 4, 5);

        System.out.println("Numbers:                " + numbers);
        System.out.println("Sum (reduce):           " + sumWithReduce(numbers) + "   (expected: 15)");
        System.out.println("Product (reduce):       " + productWithReduce(numbers) + "   (expected: 120)");
        System.out.println("Max (reduce):           " + maxWithReduce(numbers) + "   (expected: 5)");
        System.out.println("Sum (parallel reduce):  " + sumWithParallelReduce(numbers) + "   (expected: 15)");

        List<String> words = List.of("the", "quick", "brown", "fox");
        System.out.println("\nWords:                  " + words);
        System.out.println("Concatenated (reduce):  \"" + concatenateWithReduce(words) + "\"");
    }
}
