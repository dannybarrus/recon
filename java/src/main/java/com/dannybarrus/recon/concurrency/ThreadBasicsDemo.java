package com.dannybarrus.recon.concurrency;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Demonstrates three ways to create a thread, and the single most common
 * beginner mistake: calling {@code run()} directly instead of
 * {@code start()}.
 */
public final class ThreadBasicsDemo {

    private ThreadBasicsDemo() { }

    public static void main(String[] args) throws InterruptedException {
        demoThreeWaysToCreateAThread();
        System.out.println();
        demoStartVsRunGotcha();
        System.out.println();
        demoJoinWaitsForCompletion();
    }

    private static void demoThreeWaysToCreateAThread() throws InterruptedException {
        System.out.println("--- Three ways to create a thread ---");

        Thread t1 = new Thread() {
            @Override
            public void run() {
                System.out.println("  [Thread subclass] running on: " + Thread.currentThread().getName());
            }
        };

        Runnable task = () -> System.out.println("  [Runnable] running on: " + Thread.currentThread().getName());
        Thread t2 = new Thread(task);

        Thread t3 = new Thread(() ->
            System.out.println("  [Lambda] running on: " + Thread.currentThread().getName()));

        t1.start();
        t1.join();
        t2.start();
        t2.join();
        t3.start();
        t3.join();
    }

    private static void demoStartVsRunGotcha() throws InterruptedException {
        System.out.println("--- The gotcha: start() vs run() ---");

        Thread thread1 = new Thread(() ->
            System.out.println("  Running on: " + Thread.currentThread().getName()));

        System.out.println("Calling run() directly (WRONG):");
        thread1.run();
        System.out.println("  Notice the thread name above matches the caller, not a new thread.");

        Thread thread2 = new Thread(() ->
            System.out.println("  Running on: " + Thread.currentThread().getName()));

        System.out.println("\nCalling start() (CORRECT):");
        thread2.start();
        thread2.join();
        System.out.println("  This time the thread name above is a genuinely new thread.");
    }

    private static void demoJoinWaitsForCompletion() throws InterruptedException {
        System.out.println("--- join() blocks the caller until the thread finishes ---");

        List<String> trace = Collections.synchronizedList(new ArrayList<>());

        Thread worker = new Thread(() -> {
            trace.add("worker started");
            try {
                Thread.sleep(100);
            } catch (InterruptedException ignored) { }
            trace.add("worker finished");
        });

        trace.add("main: starting worker");
        worker.start();
        trace.add("main: calling join() -- will block here until worker finishes");
        worker.join();
        trace.add("main: join() returned, worker is done");

        trace.forEach(line -> System.out.println("  " + line));
    }
}
