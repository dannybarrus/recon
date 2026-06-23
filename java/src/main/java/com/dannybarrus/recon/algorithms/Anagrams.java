package com.dannybarrus.recon.algorithms;

/**
 * Given two strings of lowercase letters (a-z), how many characters must be
 * removed (from either string) to make the two strings anagrams of each other?
 *
 * <p>Example: {@code "hello"} and {@code "billion"} -> 6
 *
 * <p>Approach: build a 26-element histogram of character counts for each
 * string. The number of removals needed equals the sum of the absolute
 * differences between the two histograms, position by position. Each unit
 * of difference for a given letter represents one character with no
 * matching partner in the other string, and must be removed.
 *
 * <p>Recreated from an original NetBeans project. The algorithm was already
 * correct; this version removes the blocking stdin read in main() (none of
 * the other examples in this repo block on console input, which also makes
 * automated builds and CI smoke tests straightforward) and adds input
 * validation for non-lowercase characters, which the original did not have.
 */
public final class Anagrams {

    private static final int ALPHABET_SIZE = 26;

    private Anagrams() { }

    /** Returns the number of character removals needed to make the two strings anagrams. */
    public static int minRemovalsToAnagram(String first, String second) {
        int[] counts1 = countCharacters(first);
        int[] counts2 = countCharacters(second);
        return computeDelta(counts1, counts2);
    }

    /** Builds a 26-element histogram of lowercase letter frequencies. */
    static int[] countCharacters(String s) {
        int[] counts = new int[ALPHABET_SIZE];
        for (int i = 0; i < s.length(); i++) {
            char c = s.charAt(i);
            int index = c - 'a';
            if (index < 0 || index >= ALPHABET_SIZE) {
                throw new IllegalArgumentException(
                    "Expected lowercase a-z only, found '" + c + "' in \"" + s + "\"");
            }
            counts[index]++;
        }
        return counts;
    }

    /** Sums the absolute difference between two equal-length histograms. */
    static int computeDelta(int[] counts1, int[] counts2) {
        if (counts1.length != counts2.length) {
            throw new IllegalArgumentException("Histograms must be the same length");
        }
        int delta = 0;
        for (int i = 0; i < counts1.length; i++) {
            delta += Math.abs(counts1[i] - counts2[i]);
        }
        return delta;
    }

    public static void main(String[] args) {
        printExample("hello", "billion");
        printExample("listen", "silent");
        printExample("abc", "abc");
        printExample("abc", "xyz");
    }

    private static void printExample(String first, String second) {
        int removals = minRemovalsToAnagram(first, second);
        System.out.printf("\"%s\" and \"%s\" -> %d removal(s) needed%n", first, second, removals);
    }
}
