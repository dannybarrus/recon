package com.dannybarrus.recon.concurrency;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Uses an explicit {@code ReentrantLock} instead of the {@code synchronized}
 * keyword.
 *
 * <p>Why reach for {@code ReentrantLock} over {@code synchronized}:
 * <ul>
 *   <li>{@code tryLock()} with a timeout -- give up rather than block forever</li>
 *   <li>interruptible locking via {@code lockInterruptibly()}</li>
 *   <li>a {@code Lock} can be passed around or held across method boundaries</li>
 *   <li>explicit fairness policy ({@code new ReentrantLock(true)})</li>
 * </ul>
 *
 * <p>The discipline that matters: <b>always unlock in a finally block.</b>
 * If the code between {@code lock()} and {@code unlock()} throws, skipping
 * the finally means the lock is held forever -- every other thread waiting
 * on it blocks indefinitely. This is the same "always run cleanup" lesson
 * as try-with-resources and the finally-gotcha demo in the exceptions
 * package, applied to a resource that has no compiler-enforced help the
 * way {@code AutoCloseable} does.
 */
public class BankAccountWithLock {

    private final Lock lock = new ReentrantLock();
    private double balance;

    public BankAccountWithLock(double initialBalance) {
        this.balance = initialBalance;
    }

    public void deposit(double amount) {
        lock.lock();
        try {
            balance += amount;
        } finally {
            lock.unlock();
        }
    }

    public boolean withdraw(double amount) {
        lock.lock();
        try {
            if (amount > balance) {
                return false;
            }
            balance -= amount;
            return true;
        } finally {
            lock.unlock();
        }
    }

    /** Attempts to withdraw, but gives up after the timeout rather than blocking forever. */
    public boolean tryWithdraw(double amount, long timeout, TimeUnit unit) throws InterruptedException {
        if (!lock.tryLock(timeout, unit)) {
            return false;
        }
        try {
            if (amount > balance) {
                return false;
            }
            balance -= amount;
            return true;
        } finally {
            lock.unlock();
        }
    }

    public double getBalance() {
        lock.lock();
        try {
            return balance;
        } finally {
            lock.unlock();
        }
    }

    /**
     * FOR DEMONSTRATION ONLY: holds the lock for the duration of
     * {@code action}, used by {@code LockDemo} to simulate real lock
     * contention so {@code tryWithdraw}'s timeout behaviour can be shown
     * against an actually-held lock, not just a fast no-op case.
     */
    void holdLockAndRun(Runnable action) {
        lock.lock();
        try {
            action.run();
        } finally {
            lock.unlock();
        }
    }
}
