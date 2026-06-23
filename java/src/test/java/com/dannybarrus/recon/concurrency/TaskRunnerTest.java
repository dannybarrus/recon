package com.dannybarrus.recon.concurrency;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.TimeUnit;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class TaskRunnerTest {

    @Test
    @Timeout(value = 10, unit = TimeUnit.SECONDS)
    void runAll_returnsResultsInSubmissionOrder() throws InterruptedException {
        List<Callable<Integer>> tasks = List.of(
            () -> 1,
            () -> 4,
            () -> 9
        );

        List<Integer> results = TaskRunner.runAll(tasks, 2);

        assertEquals(List.of(1, 4, 9), results);
    }

    @Test
    @Timeout(value = 10, unit = TimeUnit.SECONDS)
    void runAll_singleTask_singlePoolThread_stillWorks() throws InterruptedException {
        List<Callable<String>> tasks = List.of(() -> "only task");
        assertEquals(List.of("only task"), TaskRunner.runAll(tasks, 1));
    }

    @Test
    @Timeout(value = 10, unit = TimeUnit.SECONDS)
    void runAll_taskThrows_wrapsAndPropagates() {
        List<Callable<Integer>> tasks = List.of(
            () -> { throw new IllegalStateException("task failure"); }
        );

        RuntimeException ex = assertThrows(RuntimeException.class,
            () -> TaskRunner.runAll(tasks, 1));

        assertEquals("Task failed", ex.getMessage());
        assertEquals(IllegalStateException.class, ex.getCause().getClass());
    }
}
