package com.dannybarrus.recon.concurrency;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;

import static org.junit.jupiter.api.Assertions.assertEquals;

class CompletableFutureExamplesTest {

    @Test
    @Timeout(value = 5, unit = TimeUnit.SECONDS)
    void doubleAsync_doublesTheValue() throws ExecutionException, InterruptedException {
        assertEquals(42, CompletableFutureExamples.doubleAsync(21).get());
    }

    @Test
    @Timeout(value = 5, unit = TimeUnit.SECONDS)
    void fetchUserThenFetchOrders_chainsBothAsyncCalls() throws ExecutionException, InterruptedException {
        assertEquals("user-7 has 3 orders", CompletableFutureExamples.fetchUserThenFetchOrders(7).get());
    }

    @Test
    @Timeout(value = 5, unit = TimeUnit.SECONDS)
    void sumOfTwoAsyncValues_combinesBothResults() throws ExecutionException, InterruptedException {
        assertEquals(7, CompletableFutureExamples.sumOfTwoAsyncValues(3, 4).get());
    }

    @Test
    @Timeout(value = 5, unit = TimeUnit.SECONDS)
    void divideWithFallback_validDivision_returnsResult() throws ExecutionException, InterruptedException {
        assertEquals(5, CompletableFutureExamples.divideWithFallback(10, 2).get());
    }

    @Test
    @Timeout(value = 5, unit = TimeUnit.SECONDS)
    void divideWithFallback_divideByZero_recoversWithFallback() throws ExecutionException, InterruptedException {
        assertEquals(-1, CompletableFutureExamples.divideWithFallback(10, 0).get());
    }
}
