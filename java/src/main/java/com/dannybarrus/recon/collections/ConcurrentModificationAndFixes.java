package com.dannybarrus.recon.collections;

import java.util.ArrayList;
import java.util.ConcurrentModificationException;
import java.util.Iterator;
import java.util.List;

/**
 * Demonstrates {@link ConcurrentModificationException} -- one of the most
 * common runtime surprises in Java collections -- and three correct ways
 * to avoid it.
 *
 * <p>The problem: most {@code java.util} collections use a "fail-fast"
 * iterator that tracks a modification count. If the underlying collection
 * is structurally modified (add or remove, not just changing an existing
 * element's value) while a for-each loop or explicit {@code Iterator} is in
 * progress, the next call to {@code next()} throws
 * {@code ConcurrentModificationException} -- even in single-threaded code.
 * The exception's name is misleading; it is not specifically about threads.
 */
public final class ConcurrentModificationAndFixes {

    private ConcurrentModificationAndFixes() { }

    /**
     * The broken approach: removing from a list while iterating it with
     * a for-each loop (which uses an Iterator under the hood).
     */
    public static void removeEvens_broken(List<Integer> list) {
        for (Integer value : list) {
            if (value % 2 == 0) {
                list.remove(value);   // structurally modifies the list mid-iteration
            }
        }
    }

    /**
     * Fix 1: use the Iterator's own remove() method. This is the only safe
     * way to remove elements while iterating with an explicit Iterator --
     * it updates the iterator's internal state so no exception is thrown.
     */
    public static void removeEvens_iteratorRemove(List<Integer> list) {
        Iterator<Integer> it = list.iterator();
        while (it.hasNext()) {
            if (it.next() % 2 == 0) {
                it.remove();
            }
        }
    }

    /**
     * Fix 2: removeIf(), added in Java 8. The cleanest option when the
     * condition can be expressed as a simple predicate -- no explicit
     * iterator management needed at all.
     */
    public static void removeEvens_removeIf(List<Integer> list) {
        list.removeIf(value -> value % 2 == 0);
    }

    /**
     * Fix 3: iterate over a copy, mutate the original. Useful when the
     * removal logic is complex enough that removeIf's single predicate
     * does not cleanly express it, or when you need to do other work
     * during the loop body beyond just deciding whether to remove.
     */
    public static void removeEvens_iterateCopy(List<Integer> list) {
        List<Integer> snapshot = new ArrayList<>(list);
        for (Integer value : snapshot) {
            if (value % 2 == 0) {
                list.remove(value);   // mutating the ORIGINAL is fine --
                                       // we are iterating the SNAPSHOT
            }
        }
    }

    public static void main(String[] args) {
        System.out.println("--- The broken approach ---");
        try {
            removeEvens_broken(sample());
            System.out.println("(did not throw -- this can happen with small lists; do not rely on it)");
        } catch (ConcurrentModificationException e) {
            System.out.println("Threw ConcurrentModificationException, as expected.");
        }

        System.out.println("\n--- Fix 1: Iterator.remove() ---");
        List<Integer> list1 = sample();
        removeEvens_iteratorRemove(list1);
        System.out.println("Result: " + list1 + "   (expected: odds only -- [1, 3, 5, 7, 9])");

        System.out.println("\n--- Fix 2: List.removeIf() ---");
        List<Integer> list2 = sample();
        removeEvens_removeIf(list2);
        System.out.println("Result: " + list2 + "   (expected: [1, 3, 5, 7, 9])");

        System.out.println("\n--- Fix 3: iterate a copy, mutate the original ---");
        List<Integer> list3 = sample();
        removeEvens_iterateCopy(list3);
        System.out.println("Result: " + list3 + "   (expected: [1, 3, 5, 7, 9])");
    }

    private static List<Integer> sample() {
        List<Integer> list = new ArrayList<>();
        for (int i = 1; i <= 10; i++) {
            list.add(i);
        }
        return list;
    }
}
