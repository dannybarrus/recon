package com.dannybarrus.recon.streams;

import java.util.List;
import java.util.NoSuchElementException;
import java.util.Optional;

/**
 * Demonstrates {@code Optional} done right versus done wrong.
 *
 * <p>{@code Optional} exists to make "this might not have a value" explicit
 * in a method's return type, so callers are forced to think about the
 * absent case instead of accidentally dereferencing a null. It is most
 * useful as a <b>return type</b> for methods that might not produce a
 * result.
 *
 * <p>It is generally considered poor practice to use {@code Optional} as:
 * <ul>
 *   <li>a field type on a class (use a nullable field, or better, avoid
 *       nullability in the domain model entirely),</li>
 *   <li>a method parameter type (overloading or a sensible default is
 *       almost always clearer to the caller),</li>
 *   <li>inside a collection, e.g. {@code List<Optional<T>>} (filter out
 *       absent values before collecting instead).</li>
 * </ul>
 * Optional's job is to be the last thing a method hands back, not
 * something that propagates through a whole codebase.
 */
public final class OptionalUsage {

    private OptionalUsage() { }

    /** Returns the first even number, or empty if there isn't one. */
    public static Optional<Integer> firstEven(List<Integer> numbers) {
        return numbers.stream()
            .filter(n -> n % 2 == 0)
            .findFirst();
    }

    /** Division that returns empty instead of throwing on divide-by-zero. */
    public static Optional<Integer> safeDivide(int numerator, int denominator) {
        if (denominator == 0) {
            return Optional.empty();
        }
        return Optional.of(numerator / denominator);
    }

    public static void main(String[] args) {
        demoTheWrongWay();
        System.out.println();
        demoTheRightWay();
    }

    private static void demoTheWrongWay() {
        System.out.println("--- The wrong way: get() without checking ---");

        Optional<Integer> empty = firstEven(List.of(1, 3, 5, 7));

        try {
            int value = empty.get();   // throws if empty -- this is the trap
            System.out.println("(did not throw -- this should not happen): " + value);
        } catch (NoSuchElementException e) {
            System.out.println("get() on an empty Optional threw NoSuchElementException.");
            System.out.println("isPresent()-then-get() is only marginally better -- it's the same");
            System.out.println("race-prone check-then-act pattern null checks have always had.");
        }
    }

    private static void demoTheRightWay() {
        System.out.println("--- The right way: don't ever call get() blind ---");

        Optional<Integer> empty = firstEven(List.of(1, 3, 5, 7));
        Optional<Integer> present = firstEven(List.of(1, 2, 3, 4));

        System.out.println("orElse(-1) on empty:        " + empty.orElse(-1));

        System.out.println("orElseGet(...) on empty:    " + empty.orElseGet(() -> {
            System.out.print("(computing default) ");
            return -1;
        }));

        System.out.println("present.map(x -> x * 100):  " + present.map(x -> x * 100).orElse(0));
        System.out.println("empty.map(x -> x * 100):    " + empty.map(x -> x * 100).orElse(0));

        present.ifPresent(x -> System.out.println("ifPresent ran with value: " + x));
        empty.ifPresentOrElse(
            x -> System.out.println("present branch: " + x),
            () -> System.out.println("ifPresentOrElse: empty branch ran instead"));

        try {
            empty.orElseThrow(() -> new IllegalStateException("Expected at least one even number"));
        } catch (IllegalStateException e) {
            System.out.println("orElseThrow(custom): " + e.getMessage());
        }

        System.out.println("\nsafeDivide(10, 2):  " + safeDivide(10, 2).orElse(-1));
        System.out.println("safeDivide(10, 0):  " + safeDivide(10, 0).orElse(-1) + "   (no exception, no null)");
    }
}
