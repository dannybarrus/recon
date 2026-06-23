package com.dannybarrus.recon.concurrency;

/**
 * BROKEN: increments a plain {@code int} with no synchronization.
 * {@code count++} is not atomic -- it is read-increment-write, and two
 * threads can interleave those three steps and lose an increment. Mirrors
 * the {@code AddPipeline} race documented in this repo's Go bloom filter
 * project -- same category of bug, different language.
 */
public class UnsafeCounter {

    private int count = 0;

    public void increment() {
        count++;
    }

    public int get() {
        return count;
    }
}
