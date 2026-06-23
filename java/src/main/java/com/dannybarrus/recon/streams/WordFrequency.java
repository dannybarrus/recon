package com.dannybarrus.recon.streams;

import java.util.Arrays;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * A basic filter / map / collect pipeline: word frequency counting and
 * top-N extraction.
 *
 * <p>This is the canonical "first real stream pipeline" most people write.
 * It chains exactly the three core stream operations in the order they are
 * almost always used:
 * <ol>
 *   <li><b>filter</b> -- discard elements that do not belong (empty tokens)</li>
 *   <li><b>map</b> -- transform each element (normalize case)</li>
 *   <li><b>collect</b> -- reduce the whole stream into a final result
 *       (here, a frequency map)</li>
 * </ol>
 */
public final class WordFrequency {

    private WordFrequency() { }

    /** Splits text on whitespace and counts occurrences of each lowercase word. */
    public static Map<String, Long> wordFrequency(String text) {
        return Arrays.stream(text.split("\\s+"))
            .filter(word -> !word.isBlank())
            .map(String::toLowerCase)
            .collect(Collectors.groupingBy(word -> word, Collectors.counting()));
    }

    /**
     * Returns the top n most frequent words, in descending order of count.
     * Uses a LinkedHashMap so insertion order (which is sorted order here)
     * is preserved for the caller -- a plain HashMap would not guarantee that.
     */
    public static Map<String, Long> topNWords(String text, int n) {
        return wordFrequency(text).entrySet().stream()
            .sorted(Map.Entry.<String, Long>comparingByValue().reversed())
            .limit(n)
            .collect(Collectors.toMap(
                Map.Entry::getKey,
                Map.Entry::getValue,
                (a, b) -> a,            // merge function -- unused since keys are already distinct
                LinkedHashMap::new));   // preserves the sorted order from the stream
    }

    public static void main(String[] args) {
        String text = "the quick brown fox jumps over the lazy dog the fox runs the dog barks";

        System.out.println("--- Full word frequency ---");
        wordFrequency(text).forEach((word, count) ->
            System.out.printf("  %-8s %d%n", word, count));

        System.out.println("\n--- Top 3 words ---");
        topNWords(text, 3).forEach((word, count) ->
            System.out.printf("  %-8s %d%n", word, count));
    }
}
