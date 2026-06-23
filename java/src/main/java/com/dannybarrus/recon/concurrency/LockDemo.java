package com.dannybarrus.recon.concurrency;

import java.util.concurrent.TimeUnit;

public final class LockDemo {

    private LockDemo() { }

    public static void main(String[] args) throws InterruptedException {
        demoConcurrentDepositsAndWithdrawals();
        System.out.println();
        demoTryLockTimeout();
    }

    private static void demoConcurrentDepositsAndWithdrawals() throws InterruptedException {
        System.out.println("--- Concurrent deposits/withdrawals via ReentrantLock ---");

        BankAccountWithLock account = new BankAccountWithLock(1000.0);
        int threadCount = 10;
        Thread[] threads = new Thread[threadCount];

        for (int i = 0; i < threadCount; i++) {
            threads[i] = new Thread(() -> {
                for (int j = 0; j < 1000; j++) {
                    account.deposit(1.0);
                    account.withdraw(1.0);
                }
            });
        }

        for (Thread t : threads) t.start();
        for (Thread t : threads) t.join();

        System.out.println("  Final balance: " + account.getBalance()
            + "   (expected: 1000.0 -- every deposit was matched by a withdrawal)");
    }

    private static void demoTryLockTimeout() throws InterruptedException {
        System.out.println("--- tryLock() with timeout: giving up rather than blocking forever ---");

        BankAccountWithLock account = new BankAccountWithLock(100.0);

        Thread holder = new Thread(() ->
            account.holdLockAndRun(() -> sleepQuietly(2000)));
        holder.start();
        Thread.sleep(200);

        long start = System.currentTimeMillis();
        boolean result = account.tryWithdraw(10.0, 500, TimeUnit.MILLISECONDS);
        long elapsed = System.currentTimeMillis() - start;

        System.out.println("  tryWithdraw while lock is held elsewhere: " + result
            + "   (expected: false -- gave up after ~500ms rather than waiting the full 2s)");
        System.out.println("  Actual wait: " + elapsed + "ms");

        holder.join();
    }

    private static void sleepQuietly(long millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException ignored) { }
    }
}
