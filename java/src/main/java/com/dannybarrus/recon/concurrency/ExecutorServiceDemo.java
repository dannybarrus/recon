package com.dannybarrus.recon.concurrency;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

public final class ExecutorServiceDemo {

    private ExecutorServiceDemo() { }

    public static void main(String[] args) throws InterruptedException {
        demoSubmitVsExecute();
        System.out.println();
        demoTaskRunnerPattern();
    }

    private static void demoSubmitVsExecute() throws InterruptedException {
        System.out.println("--- execute() vs submit() ---");

        ExecutorService executor = Executors.newFixedThreadPool(2);

        executor.execute(() -> System.out.println("  [execute] no return value available"));

        Future<String> future = executor.submit(() -> {
            Thread.sleep(50);
            return "result from submit()";
        });

        try {
            System.out.println("  [submit] " + future.get());
        } catch (Exception e) {
            System.out.println("  submit() task failed: " + e.getMessage());
        }

        executor.shutdown();
        executor.awaitTermination(5, TimeUnit.SECONDS);
    }

    private static void demoTaskRunnerPattern() throws InterruptedException {
        System.out.println("--- TaskRunner: submit a batch, collect results, always shut down ---");

        List<Callable<Integer>> tasks = new ArrayList<>();
        for (int i = 1; i <= 5; i++) {
            final int n = i;
            tasks.add(() -> n * n);
        }

        List<Integer> results = TaskRunner.runAll(tasks, 3);
        System.out.println("  Results: " + results + "   (expected: [1, 4, 9, 16, 25])");
    }
}
