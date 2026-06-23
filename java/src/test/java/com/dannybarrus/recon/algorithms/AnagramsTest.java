package com.dannybarrus.recon.algorithms;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class AnagramsTest {

    @Test
    void minRemovals_documentedExample_returnsSix() {
        assertEquals(6, Anagrams.minRemovalsToAnagram("hello", "billion"));
    }

    @Test
    void minRemovals_trueAnagrams_returnsZero() {
        assertEquals(0, Anagrams.minRemovalsToAnagram("listen", "silent"));
    }

    @Test
    void minRemovals_identicalStrings_returnsZero() {
        assertEquals(0, Anagrams.minRemovalsToAnagram("abc", "abc"));
    }

    @Test
    void minRemovals_disjointStrings_returnsSumOfLengths() {
        assertEquals(6, Anagrams.minRemovalsToAnagram("abc", "xyz"));
    }

    @Test
    void minRemovals_emptyStrings_returnsZero() {
        assertEquals(0, Anagrams.minRemovalsToAnagram("", ""));
    }

    @Test
    void minRemovals_oneEmptyString_returnsLengthOfOther() {
        assertEquals(5, Anagrams.minRemovalsToAnagram("hello", ""));
    }

    @Test
    void countCharacters_rejectsUppercase() {
        assertThrows(IllegalArgumentException.class, () -> Anagrams.countCharacters("Hello"));
    }

    @Test
    void countCharacters_rejectsNonLetters() {
        assertThrows(IllegalArgumentException.class, () -> Anagrams.countCharacters("a-b"));
    }

    @Test
    void countCharacters_countsFrequenciesCorrectly() {
        int[] counts = Anagrams.countCharacters("aab");
        assertEquals(2, counts['a' - 'a']);
        assertEquals(1, counts['b' - 'a']);
        assertEquals(0, counts['c' - 'a']);
    }
}
