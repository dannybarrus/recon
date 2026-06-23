package com.dannybarrus.recon.collections;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Demonstrates the three common ways to get an "immutable" list in Java,
 * and the gotcha that catches people who assume all three behave the same.
 *
 * <ul>
 *   <li>{@code List.of(...)} -- genuinely immutable. Any mutation attempt
 *       throws {@code UnsupportedOperationException}.</li>
 *   <li>{@code new ArrayList<>(source)} -- a mutable defensive copy.
 *       Mutating it never affects {@code source}, and vice versa.</li>
 *   <li>{@code Collections.unmodifiableList(source)} -- <b>not actually
 *       immutable.</b> It is a read-only <em>view</em> over {@code source}.
 *       You cannot mutate it directly, but if you still hold a reference to
 *       the original {@code source} list, mutating that still changes what
 *       the "unmodifiable" view shows. This surprises people constantly.</li>
 * </ul>
 */
public final class ImmutableCollections {

    private ImmutableCollections() { }

    /**
     * Returns a genuinely immutable defensive copy of the input list.
     * Safe to hand out to callers without worrying about either side
     * mutating data the other depends on.
     */
    public static List<String> defensiveImmutableCopy(List<String> source) {
        return List.copyOf(source);
    }

    public static void main(String[] args) {
        demoListOf();
        System.out.println();
        demoMutableCopy();
        System.out.println();
        demoUnmodifiableListGotcha();
    }

    private static void demoListOf() {
        System.out.println("--- List.of() -- genuinely immutable ---");
        List<String> immutable = List.of("a", "b", "c");
        try {
            immutable.add("d");
            System.out.println("(did not throw -- this should not happen)");
        } catch (UnsupportedOperationException e) {
            System.out.println("add() threw UnsupportedOperationException, as expected.");
        }
    }

    private static void demoMutableCopy() {
        System.out.println("--- new ArrayList<>(source) -- independent mutable copy ---");
        List<String> source = new ArrayList<>(List.of("a", "b", "c"));
        List<String> copy = new ArrayList<>(source);

        copy.add("d");
        source.add("z");

        System.out.println("source: " + source + "   (unaffected by changes to copy)");
        System.out.println("copy:   " + copy + "   (unaffected by changes to source)");
    }

    private static void demoUnmodifiableListGotcha() {
        System.out.println("--- Collections.unmodifiableList() -- the gotcha ---");

        List<String> source = new ArrayList<>(List.of("a", "b", "c"));
        List<String> view = Collections.unmodifiableList(source);

        System.out.println("view:   " + view);

        try {
            view.add("d");
            System.out.println("(did not throw -- this should not happen)");
        } catch (UnsupportedOperationException e) {
            System.out.println("view.add() threw UnsupportedOperationException -- looks safe so far...");
        }

        // The gotcha: mutating the ORIGINAL list still changes what the view shows.
        source.add("z");
        System.out.println("After source.add(\"z\"):");
        System.out.println("view:   " + view + "   (changed! the \"unmodifiable\" view is not actually immutable)");

        System.out.println();
        System.out.println("Lesson: unmodifiableList() protects against the VIEW being mutated directly.");
        System.out.println("It does NOT protect against the backing list being mutated through another");
        System.out.println("reference. For a real immutability guarantee, use List.copyOf() or List.of().");
    }
}
