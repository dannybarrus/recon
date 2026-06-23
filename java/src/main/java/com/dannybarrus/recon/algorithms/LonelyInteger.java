package com.dannybarrus.recon.algorithms;

import java.util.Arrays;

/**
 * Given an array in which every element occurs exactly twice except for one,
 * find that one unique element.
 *
 * <p>Approach: XOR every element together. Identical values cancel each
 * other out ({@code x ^ x == 0}), so after XOR-ing the whole array, only the
 * unique value survives. O(n) time, O(1) space -- no auxiliary data
 * structure needed.
 *
 * <p>Recreated from an original NetBeans project. The algorithm and the
 * bitwise walkthrough printout were already correct and are preserved as-is.
 */
public final class LonelyInteger {

    private LonelyInteger() { }

    /** Returns the element that appears exactly once in an array where all others appear in pairs. */
    public static int findLonely(int[] values) {
        int result = 0;
        for (int value : values) {
            result ^= value;
        }
        return result;
    }

    public static void main(String[] args) {
        int[] example = {9, 1, 2, 3, 2, 9, 1, 7, 7};
        System.out.println("Array:          " + Arrays.toString(example));
        System.out.println("Lonely integer: " + findLonely(example));
        printBitwiseWalkthrough();
    }

    private static void printBitwiseWalkthrough() {
        System.out.println();
        System.out.println("Bitwise walkthrough:");
        System.out.println("  9   1001");
        System.out.println("  1   0001");
        System.out.println("  1   0001");
        System.out.println("  3   0011");
        System.out.println("  2   0010");
        System.out.println("  9   1001");
        System.out.println("  1   0001");
        System.out.println("  7   0111");
        System.out.println("  7   0111");
        System.out.println("  7   0111");
        System.out.println("  ----------  XOR all bits");
        System.out.println("  3   0011");
    }
}
