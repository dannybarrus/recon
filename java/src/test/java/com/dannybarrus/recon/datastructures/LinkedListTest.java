package com.dannybarrus.recon.datastructures;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

class LinkedListTest {

    @Test
    void newList_isEmpty() {
        LinkedList<String> list = new LinkedList<>();
        assertTrue(list.isEmpty());
        assertEquals(0, list.size());
        assertEquals("", list.toString());
    }

    @Test
    void add_appendsToEnd() {
        LinkedList<String> list = new LinkedList<>();
        list.add("a");
        list.add("b");
        list.add("c");
        assertEquals("[a][b][c]", list.toString());
        assertEquals(3, list.size());
    }

    @Test
    void get_returnsElementAtIndex() {
        LinkedList<String> list = new LinkedList<>();
        for (int i = 0; i <= 5; i++) {
            list.add(String.valueOf(i));
        }
        assertEquals("3", list.get(3));
    }

    @Test
    void get_outOfBounds_throws() {
        LinkedList<String> list = new LinkedList<>();
        list.add("a");
        assertThrows(IndexOutOfBoundsException.class, () -> list.get(5));
        assertThrows(IndexOutOfBoundsException.class, () -> list.get(-1));
    }

    @Test
    void remove_shiftsSubsequentElements() {
        LinkedList<String> list = new LinkedList<>();
        for (int i = 0; i <= 5; i++) {
            list.add(String.valueOf(i));
        }
        assertTrue(list.remove(2));
        assertEquals(5, list.size());
        assertEquals("4", list.get(3));   // was at index 4, shifted to index 3 after removal
    }

    @Test
    void remove_indexZero_worksCorrectly() {
        // The original implementation had an off-by-one bug that disallowed
        // removing index 0 (it rejected index < 1 instead of index < 0).
        LinkedList<String> list = new LinkedList<>();
        list.add("a");
        list.add("b");
        assertTrue(list.remove(0));
        assertEquals("[b]", list.toString());
    }

    @Test
    void remove_outOfBounds_returnsFalse() {
        LinkedList<String> list = new LinkedList<>();
        list.add("a");
        assertFalse(list.remove(5));
        assertFalse(list.remove(-1));
    }

    @Test
    void addAtIndex_insertsWithoutOverwriting() {
        LinkedList<String> list = new LinkedList<>();
        list.add("a");
        list.add("c");
        list.add("b", 1);
        assertEquals("[a][b][c]", list.toString());
    }

    @Test
    void addAtIndex_atEnd_behavesLikeAppend() {
        LinkedList<String> list = new LinkedList<>();
        list.add("a");
        list.add("b");
        list.add("c", 2);
        assertEquals("[a][b][c]", list.toString());
    }

    @Test
    void addAtIndex_onEmptyList_atIndexZero_works() {
        // The original implementation threw an unchecked NullPointerException
        // here, masked by @SuppressWarnings("null") rather than handled.
        LinkedList<String> list = new LinkedList<>();
        list.add("a", 0);
        assertEquals("[a]", list.toString());
    }

    @Test
    void addAtIndex_outOfBounds_throws() {
        LinkedList<String> list = new LinkedList<>();
        list.add("a");
        assertThrows(IndexOutOfBoundsException.class, () -> list.add("x", 5));
        assertThrows(IndexOutOfBoundsException.class, () -> list.add("x", -1));
    }

    @Test
    void hasCycle_onEmptyList_returnsFalse() {
        LinkedList<String> list = new LinkedList<>();
        assertFalse(list.hasCycle());
    }

    @Test
    void hasCycle_onAcyclicList_returnsFalse() {
        LinkedList<String> list = new LinkedList<>();
        for (int i = 0; i <= 5; i++) {
            list.add(String.valueOf(i));
        }
        assertFalse(list.hasCycle());
    }

    @Test
    void hasCycle_afterCreatingCycle_returnsTrue() {
        LinkedList<String> list = new LinkedList<>();
        for (int i = 0; i <= 5; i++) {
            list.add(String.valueOf(i));
        }
        list.createCyclicLink(5, 3);
        assertTrue(list.hasCycle());
    }

    @Test
    void hasCycle_selfLoop_returnsTrue() {
        LinkedList<String> list = new LinkedList<>();
        list.add("a");
        list.add("b");
        list.createCyclicLink(1, 1);
        assertTrue(list.hasCycle());
    }
}
