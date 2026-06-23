package com.dannybarrus.recon.concurrency;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.TimeUnit;

import static org.junit.jupiter.api.Assertions.assertEquals;

class SimpleBoundedBufferTest {

    @Test
    void putThenTake_returnsTheSameItem() throws InterruptedException {
        SimpleBoundedBuffer<String> buffer = new SimpleBoundedBuffer<>(5);
        buffer.put("hello");
        assertEquals("hello", buffer.take());
    }

    @Test
    void size_reflectsCurrentContents() throws InterruptedException {
        SimpleBoundedBuffer<Integer> buffer = new SimpleBoundedBuffer<>(5);
        buffer.put(1);
        buffer.put(2);
        assertEquals(2, buffer.size());
        buffer.take();
        assertEquals(1, buffer.size());
    }

    @Test
    @Timeout(value = 10, unit = TimeUnit.SECONDS)
    void producerConsumer_deliversAllItemsInOrder() throws InterruptedException {
        SimpleBoundedBuffer<Integer> buffer = new SimpleBoundedBuffer<>(3);
        List<Integer> consumed = Collections.synchronizedList(new ArrayList<>());
        int itemCount = 50;

        Thread producer = new Thread(() -> {
            try {
                for (int i = 1; i <= itemCount; i++) {
                    buffer.put(i);
                }
            } catch (InterruptedException ignored) { }
        });

        Thread consumer = new Thread(() -> {
            try {
                for (int i = 0; i < itemCount; i++) {
                    consumed.add(buffer.take());
                }
            } catch (InterruptedException ignored) { }
        });

        producer.start();
        consumer.start();
        producer.join();
        consumer.join();

        List<Integer> expected = new ArrayList<>();
        for (int i = 1; i <= itemCount; i++) {
            expected.add(i);
        }
        assertEquals(expected, consumed);
    }
}
