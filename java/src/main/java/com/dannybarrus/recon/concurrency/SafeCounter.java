package com.dannybarrus.recon.concurrency;

/**
 * CORRECT: {@code synchronized} makes {@code increment()} atomic with
 * respect to other synchronized methods on the same instance -- only one
 * thread can execute a synchronized method on a given object at a time,
 * others block until it's free.
 */
public class SafeCounter {

    private int count = 0;

    public synchronized void increment() {
        count++;
    }

    public synchronized int get() {
        return count;
    }
}
