package com.dannybarrus.recon.collections;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;

/**
 * Verifies the helper methods produce correct results. Deliberately does
 * NOT assert on timing -- see the class-level javadoc on
 * {@link CollectionPerformance} for why that would be a flaky-test
 * anti-pattern.
 */
class CollectionPerformanceTest {

    @Test
    void appendN_arrayList_producesCorrectOrderAndSize() {
        List<Integer> list = new ArrayList<>();
        CollectionPerformance.appendN(list, 5);
        assertEquals(List.of(0, 1, 2, 3, 4), list);
    }

    @Test
    void appendN_linkedList_producesCorrectOrderAndSize() {
        List<Integer> list = new LinkedList<>();
        CollectionPerformance.appendN(list, 5);
        assertEquals(List.of(0, 1, 2, 3, 4), list);
    }

    @Test
    void prependN_reversesOrder() {
        // Each prepend puts the newest value at index 0, so the final
        // order is the reverse of insertion order.
        List<Integer> list = new ArrayList<>();
        CollectionPerformance.prependN(list, 5);
        assertEquals(List.of(4, 3, 2, 1, 0), list);
    }

    @Test
    void sumByIndex_returnsCorrectSum() {
        List<Integer> list = List.of(1, 2, 3, 4, 5);
        assertEquals(15L, CollectionPerformance.sumByIndex(list));
    }

    @Test
    void sumByIndex_emptyList_returnsZero() {
        assertEquals(0L, CollectionPerformance.sumByIndex(new ArrayList<>()));
    }
}
