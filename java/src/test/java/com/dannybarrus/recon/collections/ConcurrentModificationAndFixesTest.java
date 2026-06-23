package com.dannybarrus.recon.collections;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.ConcurrentModificationException;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class ConcurrentModificationAndFixesTest {

    private static List<Integer> sample() {
        List<Integer> list = new ArrayList<>();
        for (int i = 1; i <= 10; i++) {
            list.add(i);
        }
        return list;
    }

    @Test
    void brokenApproach_throwsConcurrentModificationException() {
        assertThrows(ConcurrentModificationException.class,
            () -> ConcurrentModificationAndFixes.removeEvens_broken(sample()));
    }

    @Test
    void iteratorRemove_removesAllEvens() {
        List<Integer> list = sample();
        ConcurrentModificationAndFixes.removeEvens_iteratorRemove(list);
        assertEquals(List.of(1, 3, 5, 7, 9), list);
    }

    @Test
    void removeIf_removesAllEvens() {
        List<Integer> list = sample();
        ConcurrentModificationAndFixes.removeEvens_removeIf(list);
        assertEquals(List.of(1, 3, 5, 7, 9), list);
    }

    @Test
    void iterateCopy_removesAllEvens() {
        List<Integer> list = sample();
        ConcurrentModificationAndFixes.removeEvens_iterateCopy(list);
        assertEquals(List.of(1, 3, 5, 7, 9), list);
    }

    @Test
    void allThreeFixes_produceIdenticalResults() {
        List<Integer> a = sample();
        List<Integer> b = sample();
        List<Integer> c = sample();

        ConcurrentModificationAndFixes.removeEvens_iteratorRemove(a);
        ConcurrentModificationAndFixes.removeEvens_removeIf(b);
        ConcurrentModificationAndFixes.removeEvens_iterateCopy(c);

        assertEquals(a, b);
        assertEquals(b, c);
    }
}
