package com.dannybarrus.recon.algorithms;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class LonelyIntegerTest {

    @Test
    void findLonely_documentedExample_returnsThree() {
        int[] values = {9, 1, 2, 3, 2, 9, 1, 7, 7};
        assertEquals(3, LonelyInteger.findLonely(values));
    }

    @Test
    void findLonely_singleElement_returnsItself() {
        assertEquals(42, LonelyInteger.findLonely(new int[] {42}));
    }

    @Test
    void findLonely_lonelyAtStart_isFound() {
        assertEquals(5, LonelyInteger.findLonely(new int[] {5, 1, 1, 2, 2}));
    }

    @Test
    void findLonely_lonelyAtEnd_isFound() {
        assertEquals(5, LonelyInteger.findLonely(new int[] {1, 1, 2, 2, 5}));
    }

    @Test
    void findLonely_lonelyInMiddle_isFound() {
        assertEquals(5, LonelyInteger.findLonely(new int[] {1, 1, 5, 2, 2}));
    }

    @Test
    void findLonely_negativeNumbers_worksCorrectly() {
        assertEquals(-7, LonelyInteger.findLonely(new int[] {-7, 3, 3}));
    }
}
