package com.dannybarrus.recon.collections;

import org.junit.jupiter.api.Test;

import java.util.HashSet;
import java.util.Set;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class PointTest {

    @Test
    void equals_isReflexive() {
        Point p = new Point(1, 2);
        assertTrue(p.equals(p));
    }

    @Test
    void equals_sameCoordinates_areEqual() {
        assertEquals(new Point(1, 2), new Point(1, 2));
    }

    @Test
    void equals_differentCoordinates_areNotEqual() {
        assertNotEquals(new Point(1, 2), new Point(1, 3));
    }

    @Test
    void equals_null_returnsFalseWithoutThrowing() {
        Point p = new Point(1, 2);
        assertFalse(p.equals(null));
    }

    @Test
    void equals_differentType_returnsFalse() {
        Point p = new Point(1, 2);
        assertFalse(p.equals("not a point"));
    }

    @Test
    void hashCode_equalObjects_haveEqualHashCodes() {
        Point a = new Point(5, 7);
        Point b = new Point(5, 7);
        assertEquals(a.hashCode(), b.hashCode());
    }

    @Test
    void hashSet_deduplicatesEqualPoints() {
        Set<Point> set = new HashSet<>();
        set.add(new Point(1, 2));
        set.add(new Point(1, 2));
        assertEquals(1, set.size());
    }
}
