package com.dannybarrus.recon.concurrency;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

/**
 * Demonstrates chaining asynchronous work with {@code CompletableFuture}
 * -- {@code thenApply} (transform a result), {@code thenCompose} (chain to
 * another async operation), {@code thenCombine} (combine two independent
 * futures), and {@code exceptionally} for error handling.
 */
public final class CompletableFutureExamples {

    private CompletableFutureExamples() { }

    /** thenApply: transform the result once it's available, synchronously. */
    public static CompletableFuture<Integer> doubleAsync(int value) {
        return CompletableFuture.supplyAsync(() -> value)
            .thenApply(v -> v * 2);
    }

    /** thenCompose: chain to ANOTHER async operation that itself returns a CompletableFuture. */
    public static CompletableFuture<String> fetchUserThenFetchOrders(int userId) {
        return fetchUserName(userId)
            .thenCompose(CompletableFutureExamples::fetchOrdersSummary);
    }

    /** thenCombine: combine two INDEPENDENT futures once both complete. */
    public static CompletableFuture<Integer> sumOfTwoAsyncValues(int a, int b) {
        CompletableFuture<Integer> futureA = CompletableFuture.supplyAsync(() -> a);
        CompletableFuture<Integer> futureB = CompletableFuture.supplyAsync(() -> b);
        return futureA.thenCombine(futureB, Integer::sum);
    }

    /** exceptionally: provide a fallback value if the chain fails. */
    public static CompletableFuture<Integer> divideWithFallback(int numerator, int denominator) {
        return CompletableFuture.supplyAsync(() -> numerator / denominator)
            .exceptionally(ex -> {
                System.out.println("    (recovered from: " + ex.getCause().getClass().getSimpleName() + ")");
                return -1;
            });
    }

    private static CompletableFuture<String> fetchUserName(int userId) {
        return CompletableFuture.supplyAsync(() -> "user-" + userId);
    }

    private static CompletableFuture<String> fetchOrdersSummary(String userName) {
        return CompletableFuture.supplyAsync(() -> userName + " has 3 orders");
    }

    public static void main(String[] args) throws ExecutionException, InterruptedException {
        System.out.println("--- thenApply ---");
        System.out.println("  doubleAsync(21) -> " + doubleAsync(21).get() + "   (expected: 42)");

        System.out.println("\n--- thenCompose ---");
        System.out.println("  " + fetchUserThenFetchOrders(7).get());

        System.out.println("\n--- thenCombine ---");
        System.out.println("  sumOfTwoAsyncValues(3, 4) -> " + sumOfTwoAsyncValues(3, 4).get() + "   (expected: 7)");

        System.out.println("\n--- exceptionally: fallback on failure ---");
        System.out.println("  divideWithFallback(10, 2) -> " + divideWithFallback(10, 2).get() + "   (expected: 5)");
        System.out.println("  divideWithFallback(10, 0) -> " + divideWithFallback(10, 0).get() + "   (expected: -1, recovered)");
    }
}
