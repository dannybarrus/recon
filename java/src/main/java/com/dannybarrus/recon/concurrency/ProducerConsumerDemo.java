package com.dannybarrus.recon.concurrency;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public final class ProducerConsumerDemo {

    private ProducerConsumerDemo() { }

    private static final int ITEM_COUNT = 20;
    private static final int BUFFER_CAPACITY = 5;

    public static void main(String[] args) throws InterruptedException {
        demoHandRolledBuffer();
        System.out.println();
        demoBlockingQueue();
    }

    private static void demoHandRolledBuffer() throws InterruptedException {
        System.out.println("--- Hand-rolled producer-consumer with wait()/notifyAll() ---");

        SimpleBoundedBuffer<Integer> buffer = new SimpleBoundedBuffer<>(BUFFER_CAPACITY);
        List<Integer> consumed = Collections.synchronizedList(new ArrayList<>());

        Thread producer = new Thread(() -> {
            try {
                for (int i = 1; i <= ITEM_COUNT; i++) {
                    buffer.put(i);
                }
            } catch (InterruptedException ignored) { }
        });

        Thread consumer = new Thread(() -> {
            try {
                for (int i = 0; i < ITEM_COUNT; i++) {
                    consumed.add(buffer.take());
                }
            } catch (InterruptedException ignored) { }
        });

        producer.start();
        consumer.start();
        producer.join();
        consumer.join();

        boolean inOrder = consumed.equals(expectedSequence(ITEM_COUNT));
        System.out.println("  Consumed " + consumed.size() + " items, in order: " + inOrder);
        System.out.println("  First 5: " + consumed.subList(0, 5)
            + "  Last 5: " + consumed.subList(consumed.size() - 5, consumed.size()));
    }

    private static void demoBlockingQueue() throws InterruptedException {
        System.out.println("--- The same problem, solved with BlockingQueue ---");

        BlockingQueue<Integer> queue = new ArrayBlockingQueue<>(BUFFER_CAPACITY);
        List<Integer> consumed = Collections.synchronizedList(new ArrayList<>());

        Thread producer = new Thread(() -> {
            try {
                for (int i = 1; i <= ITEM_COUNT; i++) {
                    queue.put(i);
                }
            } catch (InterruptedException ignored) { }
        });

        Thread consumer = new Thread(() -> {
            try {
                for (int i = 0; i < ITEM_COUNT; i++) {
                    consumed.add(queue.take());
                }
            } catch (InterruptedException ignored) { }
        });

        producer.start();
        consumer.start();
        producer.join();
        consumer.join();

        boolean inOrder = consumed.equals(expectedSequence(ITEM_COUNT));
        System.out.println("  Consumed " + consumed.size() + " items, in order: " + inOrder);
        System.out.println("  Zero manual synchronization code -- BlockingQueue handles");
        System.out.println("  exactly the wait/notify dance internally, correctly, every time.");
    }

    private static List<Integer> expectedSequence(int n) {
        List<Integer> expected = new ArrayList<>();
        for (int i = 1; i <= n; i++) {
            expected.add(i);
        }
        return expected;
    }
}
