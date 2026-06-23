package com.dannybarrus.recon.datastructures;

/**
 * A single node in a singly linked list.
 *
 * <p>Recreated from an original NetBeans project. Generified from
 * {@code Object} to {@code T} -- the original used raw {@code Object} for
 * its data field, which loses type safety and forces callers to cast on
 * every read. Generics exist precisely to solve this.
 *
 * @param <T> the type of data held by this node
 */
public class Node<T> {

    private T data;
    private Node<T> next;

    public Node(T data) {
        this.data = data;
        this.next = null;
    }

    public Node(T data, Node<T> next) {
        this.data = data;
        this.next = next;
    }

    public T getData() {
        return data;
    }

    public void setData(T data) {
        this.data = data;
    }

    public Node<T> getNext() {
        return next;
    }

    public void setNext(Node<T> next) {
        this.next = next;
    }
}
