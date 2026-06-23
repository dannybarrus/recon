package com.dannybarrus.recon.exceptions;

/**
 * Throws the checked {@link InsufficientFundsException} on
 * {@link #withdraw} -- callers must handle the case where there isn't
 * enough money, by either catching it or declaring it themselves. This is
 * exactly the kind of expected, recoverable business condition checked
 * exceptions exist for.
 *
 * <p>Contrast with negative-amount validation below, which throws the
 * unchecked {@code IllegalArgumentException} -- passing a negative amount
 * is a caller error, not a legitimate business outcome, so it does not
 * need the same compiler-enforced handling.
 */
public class BankAccount {

    private double balance;

    public BankAccount(double initialBalance) {
        this.balance = initialBalance;
    }

    public double getBalance() {
        return balance;
    }

    public void deposit(double amount) {
        if (amount < 0) {
            throw new IllegalArgumentException("Deposit amount cannot be negative: " + amount);
        }
        balance += amount;
    }

    public void withdraw(double amount) throws InsufficientFundsException {
        if (amount < 0) {
            throw new IllegalArgumentException("Withdrawal amount cannot be negative: " + amount);
        }
        if (amount > balance) {
            throw new InsufficientFundsException(amount, balance);
        }
        balance -= amount;
    }
}
