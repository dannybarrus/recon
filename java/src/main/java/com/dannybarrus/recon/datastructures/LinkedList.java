package com.dannybarrus.recon.datastructures;

/**
 * A singly linked list supporting basic operations plus cycle detection.
 *
 * <p>Consolidated from two original NetBeans projects (SinglyLinkedList and
 * CyclesInALinkedList) that each duplicated their own near-identical
 * Node/LinkedList classes for two separate demos. One generic, tested
 * implementation now supports both.
 *
 * <p>Uses an explicit sentinel (dummy) head node, which never holds real
 * data -- it always points to the first real node via {@code getNext()}.
 * This avoids special-casing the "list is empty" condition throughout
 * add/get/remove. The original achieved a similar effect implicitly, by
 * having the very first {@code add()} call duplicate its own data into both
 * the head and the first visible node -- functionally workable, but fragile
 * and non-obvious. An explicit sentinel is the standard, well-documented
 * pattern for this.
 *
 * <p>Bugs found and fixed from the original:
 * <ul>
 *   <li>The size counter was {@code static} -- every instance shared one
 *       counter, so two lists in memory at once would corrupt each other's
 *       size. Now an instance field.</li>
 *   <li>{@code remove(index)} rejected {@code index < 1} instead of
 *       {@code index < 0}, silently blocking removal of the first element.</li>
 *   <li>{@code add(data, index)} threw an unchecked NullPointerException on
 *       an empty list, masked by {@code @SuppressWarnings("null")} rather
 *       than handled. Now throws a clear {@code IndexOutOfBoundsException}
 *       for any genuinely invalid index, and correctly handles index 0 on
 *       an empty list.</li>
 *   <li>The cycle-creation method used direct field access
 *       ({@code srcNode.next = destNode}) instead of the {@code setNext()}
 *       setter used everywhere else -- inconsistent encapsulation. Renamed
 *       {@code createCylclicLink} (typo) to {@code createCyclicLink} and
 *       fixed to use the setter.</li>
 * </ul>
 *
 * @param <T> the type of elements held in this list
 */
public class LinkedList<T> {

    private final Node<T> sentinel = new Node<>(null);
    private int size = 0;

    public int size() {
        return size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        Node<T> current = sentinel.getNext();
        while (current != null) {
            sb.append('[').append(current.getData()).append(']');
            current = current.getNext();
        }
        return sb.toString();
    }

    /** Appends data to the end of the list. */
    public void add(T data) {
        Node<T> newNode = new Node<>(data);
        Node<T> current = sentinel;
        while (current.getNext() != null) {
            current = current.getNext();
        }
        current.setNext(newNode);
        size++;
    }

    /**
     * Inserts data at the given index, shifting subsequent elements right.
     *
     * @throws IndexOutOfBoundsException if index is negative or greater than size
     */
    public void add(T data, int index) {
        if (index < 0 || index > size) {
            throw new IndexOutOfBoundsException("index: " + index + ", size: " + size);
        }
        Node<T> current = sentinel;
        for (int i = 0; i < index; i++) {
            current = current.getNext();
        }
        Node<T> newNode = new Node<>(data);
        newNode.setNext(current.getNext());
        current.setNext(newNode);
        size++;
    }

    /**
     * Returns the element at the given index.
     *
     * @throws IndexOutOfBoundsException if index is out of range
     */
    public T get(int index) {
        return getNode(index).getData();
    }

    /** Returns the node at the given index. Package-visible -- used by cycle utilities. */
    Node<T> getNode(int index) {
        if (index < 0 || index >= size) {
            throw new IndexOutOfBoundsException("index: " + index + ", size: " + size);
        }
        Node<T> current = sentinel.getNext();
        for (int i = 0; i < index; i++) {
            current = current.getNext();
        }
        return current;
    }

    /**
     * Removes the element at the given index.
     *
     * @return true if an element was removed, false if index was out of range
     */
    public boolean remove(int index) {
        if (index < 0 || index >= size) {
            return false;
        }
        Node<T> current = sentinel;
        for (int i = 0; i < index; i++) {
            current = current.getNext();
        }
        current.setNext(current.getNext().getNext());
        size--;
        return true;
    }

    /**
     * Creates a cycle by pointing the node at srcIndex to the node at destIndex.
     * For demonstration purposes only -- intentionally creates an invalid,
     * non-terminating list structure so {@link #hasCycle()} can be exercised.
     */
    public void createCyclicLink(int srcIndex, int destIndex) {
        Node<T> src = getNode(srcIndex);
        Node<T> dest = getNode(destIndex);
        src.setNext(dest);
    }

    /**
     * Detects a cycle using Floyd's tortoise-and-hare algorithm.
     * Two pointers traverse the list at different speeds; if a cycle
     * exists, the fast pointer eventually laps the slow pointer.
     */
    public boolean hasCycle() {
        Node<T> slow = sentinel.getNext();
        Node<T> fast = sentinel.getNext();
        while (fast != null && fast.getNext() != null) {
            slow = slow.getNext();
            fast = fast.getNext().getNext();
            if (slow == fast) {
                return true;
            }
        }
        return false;
    }
}
