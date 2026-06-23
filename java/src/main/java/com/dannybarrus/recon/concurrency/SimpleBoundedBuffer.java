package com.dannybarrus.recon.concurrency;

import java.util.LinkedList;
import java.util.Queue;

/**
 * A hand-rolled bounded buffer using {@code wait()}/{@code notifyAll()} --
 * the classic, low-level building block behind producer-consumer queues,
 * from before {@code java.util.concurrent} existed.
 *
 * <p>Two rules this class deliberately follows, both common interview
 * gotchas if missed:
 *
 * <ol>
 *   <li>The {@code wait()} call is inside a <b>while</b> loop, never an
 *       {@code if}. A thread can wake from {@code wait()} for reasons
 *       other than being notified -- a "spurious wakeup" is explicitly
 *       permitted by the JLS -- so the condition must be re-checked after
 *       waking, never assumed true.</li>
 *   <li>{@code notifyAll()} is used, not {@code notify()}. {@code notify()}
 *       wakes exactly one waiting thread, chosen arbitrarily -- if that
 *       thread turns out not to be able to proceed (multiple producers and
 *       consumers all waiting on different conditions), other threads that
 *       could have proceeded are never woken at all. {@code notifyAll()}
 *       wakes everyone, and the while-loop re-check sorts out who actually
 *       can proceed.</li>
 * </ol>
 */
public class SimpleBoundedBuffer<T> {

    private final Queue<T> queue = new LinkedList<>();
    private final int capacity;

    public SimpleBoundedBuffer(int capacity) {
        this.capacity = capacity;
    }

    public synchronized void put(T item) throws InterruptedException {
        while (queue.size() == capacity) {
            wait();
        }
        queue.add(item);
        notifyAll();
    }

    public synchronized T take() throws InterruptedException {
        while (queue.isEmpty()) {
            wait();
        }
        T item = queue.poll();
        notifyAll();
        return item;
    }

    public synchronized int size() {
        return queue.size();
    }
}
