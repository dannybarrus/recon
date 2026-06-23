package com.dannybarrus.recon.streams;

import org.junit.jupiter.api.Test;

import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class WordFrequencyTest {

    @Test
    void wordFrequency_countsCorrectly() {
        Map<String, Long> freq = WordFrequency.wordFrequency("the cat the dog the bird");
        assertEquals(3L, freq.get("the"));
        assertEquals(1L, freq.get("cat"));
        assertEquals(1L, freq.get("dog"));
        assertEquals(1L, freq.get("bird"));
    }

    @Test
    void wordFrequency_isCaseInsensitive() {
        Map<String, Long> freq = WordFrequency.wordFrequency("The THE the");
        assertEquals(3L, freq.get("the"));
        assertEquals(1, freq.size());
    }

    @Test
    void wordFrequency_ignoresExtraWhitespace() {
        Map<String, Long> freq = WordFrequency.wordFrequency("a   b\tc\n a");
        assertEquals(2L, freq.get("a"));
        assertEquals(1L, freq.get("b"));
        assertEquals(1L, freq.get("c"));
    }

    @Test
    void topNWords_returnsCorrectCountAndOrder() {
        Map<String, Long> top = WordFrequency.topNWords("a a a b b c", 2);
        assertEquals(2, top.size());
        assertTrue(top.containsKey("a"));
        assertTrue(top.containsKey("b"));
        assertEquals(3L, top.get("a"));
        assertEquals(2L, top.get("b"));
    }

    @Test
    void topNWords_nLargerThanDistinctWords_returnsAll() {
        Map<String, Long> top = WordFrequency.topNWords("a b c", 10);
        assertEquals(3, top.size());
    }
}
