package com.dannybarrus.recon.exceptions;

/**
 * Demonstrates one custom checked exception ({@link InsufficientFundsException})
 * and one custom unchecked exception ({@link InvalidOrderException}),
 * side by side.
 */
public final class CustomExceptionsDemo {

    private CustomExceptionsDemo() { }

    public static void main(String[] args) {
        demoCheckedCustomException();
        System.out.println();
        demoUncheckedCustomException();
    }

    private static void demoCheckedCustomException() {
        System.out.println("--- Custom CHECKED exception: InsufficientFundsException ---");
        BankAccount account = new BankAccount(100.00);

        try {
            account.withdraw(50.00);
            System.out.println("Withdrew $50.00, balance now: $" + account.getBalance());

            account.withdraw(100.00);
            System.out.println("(did not throw -- this should not happen)");
        } catch (InsufficientFundsException e) {
            System.out.println("Caught: " + e.getMessage());
            System.out.println("Requested: $" + e.getRequestedAmount() + "  Available: $" + e.getAvailableBalance());
        }
    }

    private static void demoUncheckedCustomException() {
        System.out.println("--- Custom UNCHECKED exception: InvalidOrderException ---");

        OrderValidator.validate("Widget", 5);
        System.out.println("Validated a normal order with no issue.");

        try {
            OrderValidator.validate("", 5);
        } catch (InvalidOrderException e) {
            System.out.println("Caught: " + e.getMessage());
        }

        try {
            OrderValidator.validate("Widget", -1);
        } catch (InvalidOrderException e) {
            System.out.println("Caught: " + e.getMessage());
        }
    }
}
