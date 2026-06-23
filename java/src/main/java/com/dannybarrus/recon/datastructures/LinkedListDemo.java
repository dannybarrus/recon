package com.dannybarrus.recon.datastructures;

/**
 * Demonstrates {@link LinkedList}: basic operations and Floyd's cycle
 * detection algorithm.
 *
 * <p>Recreated from two original NetBeans projects (SinglyLinkedList and
 * CyclesInALinkedList), each with its own main() demo. Consolidated here
 * since both demos now exercise the same underlying class.
 *
 * <p>The original cycle-detection demo had a bug: it captured
 * {@code hasCycle()} in a variable <em>before</em> creating the cycle, then
 * printed that same stale value twice. The second print statement claimed
 * "Expected: true" but would have actually printed "false" both times.
 * Fixed here by calling {@code hasCycle()} again after the cycle is created.
 */
public final class LinkedListDemo {

    private LinkedListDemo() { }

    public static void main(String[] args) {
        demoBasicOperations();
        System.out.println();
        demoCycleDetection();
    }

    private static void demoBasicOperations() {
        System.out.println("--- Basic operations ---");
        LinkedList<String> list = new LinkedList<>();
        for (int i = 0; i <= 5; i++) {
            list.add(String.valueOf(i));
        }

        System.out.println("List:        " + list);
        System.out.println("size():      " + list.size());
        System.out.println("get(3):      " + list.get(3) + "   (expected: 3)");
        System.out.println("remove(2):   " + list.remove(2) + "   (element '2' removed)");
        System.out.println("get(3):      " + list.get(3) + "   (expected: 4, shifted left after removal)");
        System.out.println("size():      " + list.size());
        System.out.println("List again:  " + list);
    }

    private static void demoCycleDetection() {
        System.out.println("--- Cycle detection (Floyd's tortoise and hare) ---");
        LinkedList<String> list = new LinkedList<>();
        for (int i = 0; i <= 5; i++) {
            list.add(String.valueOf(i));
        }

        System.out.println("List:        " + list);
        System.out.println("hasCycle():  " + list.hasCycle() + "   (expected: false)");

        list.createCyclicLink(5, 3);

        // Re-checked after creating the cycle -- the original demo printed
        // a stale pre-cycle value here instead.
        System.out.println("hasCycle():  " + list.hasCycle() + "   (expected: true, after linking index 5 -> index 3)");
    }
}
