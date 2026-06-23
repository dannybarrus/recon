package com.dannybarrus.recon.collections;

import java.util.HashSet;
import java.util.Set;

/**
 * Demonstrates the practical consequence of violating the equals/hashCode
 * contract: a {@code HashSet} silently fails to deduplicate logically-equal
 * objects.
 */
public final class EqualsAndHashCodeDemo {

    private EqualsAndHashCodeDemo() { }

    public static void main(String[] args) {
        demoCorrectContract();
        System.out.println();
        demoBrokenContract();
    }

    private static void demoCorrectContract() {
        System.out.println("--- Point (correct equals + hashCode) ---");

        Point a = new Point(1, 2);
        Point b = new Point(1, 2);   // logically equal to a, but a different object

        System.out.println("a.equals(b):        " + a.equals(b) + "   (expected: true)");
        System.out.println("a.hashCode() == b.hashCode(): "
            + (a.hashCode() == b.hashCode()) + "   (expected: true -- required by the contract)");

        Set<Point> points = new HashSet<>();
        points.add(a);
        points.add(b);   // should NOT increase the set's size -- a and b are equal

        System.out.println("Added a and b to a HashSet, size: " + points.size() + "   (expected: 1)");
    }

    private static void demoBrokenContract() {
        System.out.println("--- BrokenPoint (equals overridden, hashCode not) ---");

        BrokenPoint a = new BrokenPoint(1, 2);
        BrokenPoint b = new BrokenPoint(1, 2);

        System.out.println("a.equals(b):        " + a.equals(b) + "   (still true -- equals() itself is correct)");
        System.out.println("a.hashCode() == b.hashCode(): "
            + (a.hashCode() == b.hashCode()) + "   (almost always false -- identity-based default hashCode)");

        Set<BrokenPoint> points = new HashSet<>();
        points.add(a);
        points.add(b);

        System.out.println("Added a and b to a HashSet, size: " + points.size()
            + "   (BUG: usually 2, even though equals() says they are the same)");
        System.out.println("The HashSet looked in different buckets for a and b based on their");
        System.out.println("differing hash codes, so it never called equals() to compare them at all.");
    }
}
