package com.dannybarrus.recon.exceptions;

/**
 * A custom <b>checked</b> exception -- extends {@code Exception}, not
 * {@code RuntimeException}.
 *
 * <p>Checked is the deliberate choice here: insufficient funds is an
 * expected, recoverable business condition that callers of
 * {@link BankAccount#withdraw} should be forced to think about, not
 * something that should silently propagate the way a programming bug
 * would.
 */
public class InsufficientFundsException extends Exception {

    private final double requestedAmount;
    private final double availableBalance;

    public InsufficientFundsException(double requestedAmount, double availableBalance) {
        super(String.format(
            "Cannot withdraw $%.2f -- only $%.2f available", requestedAmount, availableBalance));
        this.requestedAmount = requestedAmount;
        this.availableBalance = availableBalance;
    }

    public double getRequestedAmount() {
        return requestedAmount;
    }

    public double getAvailableBalance() {
        return availableBalance;
    }
}
