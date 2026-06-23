package com.dannybarrus.recon.streams;

import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class ReduceExamplesTest {

    @Test
    void sumWithReduce_computesCorrectSum() {
        assertEquals(15, ReduceExamples.sumWithReduce(List.of(1, 2, 3, 4, 5)));
    }

    @Test
    void sumWithReduce_emptyList_returnsIdentity() {
        assertEquals(0, ReduceExamples.sumWithReduce(List.of()));
    }

    @Test
    void productWithReduce_computesCorrectProduct() {
        assertEquals(120, ReduceExamples.productWithReduce(List.of(1, 2, 3, 4, 5)));
    }

    @Test
    void productWithReduce_emptyList_returnsIdentity() {
        assertEquals(1, ReduceExamples.productWithReduce(List.of()));
    }

    @Test
    void maxWithReduce_findsMaximum() {
        assertEquals(9, ReduceExamples.maxWithReduce(List.of(3, 9, 1, 7, 2)));
    }

    @Test
    void maxWithReduce_emptyList_throws() {
        assertThrows(IllegalArgumentException.class, () -> ReduceExamples.maxWithReduce(List.of()));
    }

    @Test
    void sumWithParallelReduce_matchesSequentialResult() {
        List<Integer> numbers = List.of(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        assertEquals(
            ReduceExamples.sumWithReduce(numbers),
            ReduceExamples.sumWithParallelReduce(numbers));
    }

    @Test
    void concatenateWithReduce_joinsWithSpaces() {
        assertEquals("the quick brown fox",
            ReduceExamples.concatenateWithReduce(List.of("the", "quick", "brown", "fox")));
    }

    @Test
    void concatenateWithReduce_emptyList_returnsEmptyString() {
        assertEquals("", ReduceExamples.concatenateWithReduce(List.of()));
    }
}
