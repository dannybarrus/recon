package com.dannybarrus.recon.streams;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class LazyEvaluationTest {

    @Test
    void buildAndRunPipeline_producesCorrectResult() {
        List<String> trace = new ArrayList<>();
        List<Integer> result = LazyEvaluation.buildAndRunPipeline(List.of(1, 2, 3, 4), trace);
        assertEquals(List.of(20, 40), result);
    }

    @Test
    void buildAndRunPipeline_nothingExecutesBeforeTerminalOperation() {
        List<String> trace = new ArrayList<>();
        LazyEvaluation.buildAndRunPipeline(List.of(1, 2, 3, 4), trace);

        int terminalOpIndex = trace.indexOf("about to call terminal operation");
        // Nothing mentioning filter( or map( should appear before this index --
        // proves the pipeline did not run when it was merely built.
        for (int i = 0; i < terminalOpIndex; i++) {
            assertTrue(!trace.get(i).startsWith("filter(") && !trace.get(i).startsWith("map("),
                "Found pipeline execution before the terminal operation: " + trace.get(i));
        }
    }

    @Test
    void buildAndRunPipeline_filterAndMapInterleavePerElement() {
        List<String> trace = new ArrayList<>();
        LazyEvaluation.buildAndRunPipeline(List.of(2, 4), trace);

        // Both elements are even, so each filter() should be immediately
        // followed by its own map() -- not all filters first, then all maps.
        int filter2 = trace.indexOf("filter(2)");
        int map2 = trace.indexOf("map(2)");
        int filter4 = trace.indexOf("filter(4)");

        assertTrue(filter2 < map2, "map(2) should come right after filter(2)");
        assertTrue(map2 < filter4, "filter(4) should not start until element 2's pipeline finished");
    }

    @Test
    void findFirstEvenGreaterThan_stopsAfterFirstMatch() {
        List<String> trace = new ArrayList<>();
        Integer result = LazyEvaluation.findFirstEvenGreaterThan(
            List.of(1, 2, 3, 4, 5, 6, 7, 8, 9, 10), 4, trace);

        assertEquals(6, result);
        // Should have checked 1 through 6 (6 elements) and stopped -- not all 10.
        assertEquals(6, trace.size());
    }

    @Test
    void findFirstEvenGreaterThan_noMatch_returnsNull() {
        List<String> trace = new ArrayList<>();
        Integer result = LazyEvaluation.findFirstEvenGreaterThan(List.of(1, 3, 5), 10, trace);
        assertEquals(null, result);
    }

    @Test
    void firstNPowersOfTwo_returnsCorrectSequence() {
        assertEquals(List.of(1L, 2L, 4L, 8L, 16L), LazyEvaluation.firstNPowersOfTwo(5));
    }

    @Test
    void firstNPowersOfTwo_zeroElements_returnsEmptyList() {
        assertEquals(List.of(), LazyEvaluation.firstNPowersOfTwo(0));
    }
}
