package com.dannybarrus.recon.concurrency;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

/**
 * A clean, reusable pattern for running a batch of tasks on a thread pool
 * and properly shutting it down afterward -- every time, even if a task
 * throws.
 *
 * <p>The gotcha this guards against: an {@code ExecutorService} that is
 * never shut down keeps its threads alive indefinitely, which on its own
 * is enough to prevent a JVM from exiting (non-daemon threads keep the
 * process alive). "Forgot to call shutdown()" is a genuinely common
 * resource leak in real codebases.
 */
public final class TaskRunner {

    private TaskRunner() { }

    public static <T> List<T> runAll(List<Callable<T>> tasks, int poolSize) throws InterruptedException {
        ExecutorService executor = Executors.newFixedThreadPool(poolSize);
        try {
            List<Future<T>> futures = new ArrayList<>();
            for (Callable<T> task : tasks) {
                futures.add(executor.submit(task));
            }

            List<T> results = new ArrayList<>();
            for (Future<T> future : futures) {
                try {
                    results.add(future.get());
                } catch (ExecutionException e) {
                    throw new RuntimeException("Task failed", e.getCause());
                }
            }
            return results;
        } finally {
            executor.shutdown();
            if (!executor.awaitTermination(5, TimeUnit.SECONDS)) {
                executor.shutdownNow();
            }
        }
    }
}
