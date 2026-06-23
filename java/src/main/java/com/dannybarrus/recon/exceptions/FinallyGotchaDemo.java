package com.dannybarrus.recon.exceptions;

import java.util.ArrayList;
import java.util.List;

/**
 * Demonstrates two classic {@code finally}-block behaviours -- one
 * harmless and expected, one a genuine, serious gotcha.
 */
public final class FinallyGotchaDemo {

    private FinallyGotchaDemo() { }

    /** finally always runs, even though try has already returned a value. */
    static String alwaysRunsFinally(List<String> trace) {
        try {
            trace.add("try block running");
            return "value from try";
        } finally {
            trace.add("finally block running");
        }
    }

    /**
     * THE DANGEROUS GOTCHA: a {@code return} statement inside
     * {@code finally} silently discards any exception thrown in the try
     * block. The exception is not caught, not logged, not propagated -- it
     * simply vanishes, and the method returns normally as if nothing
     * happened.
     */
    @SuppressWarnings("finally")
    static String finallyReturnSwallowsException() {
        try {
            throw new RuntimeException("This exception is about to disappear");
        } finally {
            return "finally's return value wins -- the exception above is gone";
        }
    }

    /**
     * A resource that fails on close(), used to contrast try-with-resources
     * against manual finally blocks: try-with-resources preserves both
     * exceptions via SUPPRESSED exceptions, rather than silently discarding
     * one the way a manual finally-with-return would.
     */
    static class ResourceThatFailsToClose implements AutoCloseable {
        @Override
        public void close() {
            throw new IllegalStateException("Failed to close cleanly");
        }
    }

    public static void main(String[] args) {
        demoFinallyAlwaysRuns();
        System.out.println();
        demoFinallyReturnSwallowsException();
        System.out.println();
        demoTryWithResourcesPreservesBothExceptions();
    }

    private static void demoFinallyAlwaysRuns() {
        System.out.println("--- finally always runs, even after a return in try ---");
        List<String> trace = new ArrayList<>();
        String result = alwaysRunsFinally(trace);
        trace.forEach(line -> System.out.println("  " + line));
        System.out.println("  Returned: " + result);
    }

    private static void demoFinallyReturnSwallowsException() {
        System.out.println("--- THE GOTCHA: return inside finally swallows an exception ---");
        String result = finallyReturnSwallowsException();
        System.out.println("  Result: " + result);
        System.out.println("  No exception reached this caller at all, even though the try");
        System.out.println("  block above clearly threw one. This is exactly why 'return inside");
        System.out.println("  finally' is flagged by most linters as a serious anti-pattern.");
    }

    private static void demoTryWithResourcesPreservesBothExceptions() {
        System.out.println("--- try-with-resources: both exceptions preserved, none silently lost ---");
        try {
            try (ResourceThatFailsToClose resource = new ResourceThatFailsToClose()) {
                throw new RuntimeException("Primary failure inside the try block");
            }
        } catch (RuntimeException e) {
            System.out.println("  Primary exception: " + e.getMessage());
            for (Throwable suppressed : e.getSuppressed()) {
                System.out.println("  Suppressed exception: " + suppressed.getMessage());
            }
            System.out.println("  Unlike manual finally blocks, try-with-resources attaches the");
            System.out.println("  close() failure as a SUPPRESSED exception rather than letting it");
            System.out.println("  silently replace or discard the original.");
        }
    }
}
