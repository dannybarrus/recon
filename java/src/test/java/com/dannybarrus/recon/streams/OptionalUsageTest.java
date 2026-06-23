package com.dannybarrus.recon.streams;

import org.junit.jupiter.api.Test;

import java.util.List;
import java.util.Optional;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class OptionalUsageTest {

    @Test
    void firstEven_found_returnsPresent() {
        Optional<Integer> result = OptionalUsage.firstEven(List.of(1, 3, 4, 5, 6));
        assertTrue(result.isPresent());
        assertEquals(4, result.get());
    }

    @Test
    void firstEven_notFound_returnsEmpty() {
        Optional<Integer> result = OptionalUsage.firstEven(List.of(1, 3, 5, 7));
        assertFalse(result.isPresent());
    }

    @Test
    void firstEven_emptyList_returnsEmpty() {
        assertFalse(OptionalUsage.firstEven(List.of()).isPresent());
    }

    @Test
    void safeDivide_validDivision_returnsCorrectResult() {
        assertEquals(5, OptionalUsage.safeDivide(10, 2).orElseThrow());
    }

    @Test
    void safeDivide_byZero_returnsEmpty() {
        assertTrue(OptionalUsage.safeDivide(10, 0).isEmpty());
    }

    @Test
    void safeDivide_truncatesTowardZero_likeIntegerDivision() {
        assertEquals(3, OptionalUsage.safeDivide(10, 3).orElseThrow());
    }
}
