package com.dannybarrus.recon.exceptions;

/**
 * Demonstrates exception chaining done right, and the anti-pattern of
 * doing it wrong by discarding the original cause.
 */
public final class ExceptionChainingDemo {

    private ExceptionChainingDemo() { }

    /** Simulates a low-level data access call that can fail in a low-level way. */
    static int loadUserId(String rawValue) {
        try {
            return Integer.parseInt(rawValue);
        } catch (NumberFormatException e) {
            throw new DataAccessException(
                "Failed to load user ID from raw value: \"" + rawValue + "\"", e);
        }
    }

    public static void main(String[] args) {
        demoSuccessfulLoad();
        System.out.println();
        demoChainedFailure();
        System.out.println();
        demoTheAntiPattern_swallowingTheCause();
    }

    private static void demoSuccessfulLoad() {
        System.out.println("--- Successful load, no exception involved ---");
        System.out.println("loadUserId(\"42\") -> " + loadUserId("42"));
    }

    private static void demoChainedFailure() {
        System.out.println("--- Failure, properly chained ---");
        try {
            loadUserId("not-a-valid-id");
        } catch (DataAccessException e) {
            System.out.println("Caught: " + e.getMessage());
            System.out.println("Caused by: " + e.getCause().getClass().getSimpleName()
                + " -- " + e.getCause().getMessage());
            System.out.println("(The original NumberFormatException is still fully available");
            System.out.println("via getCause() -- nothing about the root cause was lost.)");
        }
    }

    private static void demoTheAntiPattern_swallowingTheCause() {
        System.out.println("--- The anti-pattern: re-throwing WITHOUT the cause ---");
        try {
            try {
                Integer.parseInt("also-not-a-number");
            } catch (NumberFormatException e) {
                throw new DataAccessException("Could not process value", null);
            }
        } catch (DataAccessException e) {
            System.out.println("Caught: " + e.getMessage());
            System.out.println("getCause(): " + e.getCause()
                + "   (null -- the real failure reason is permanently lost)");
        }
    }
}
