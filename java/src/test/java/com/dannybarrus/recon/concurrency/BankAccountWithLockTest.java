package com.dannybarrus.recon.concurrency;

import org.junit.jupiter.api.Test;

import java.util.concurrent.TimeUnit;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class BankAccountWithLockTest {

    @Test
    void deposit_increasesBalance() {
        BankAccountWithLock account = new BankAccountWithLock(100);
        account.deposit(50);
        assertEquals(150, account.getBalance());
    }

    @Test
    void withdraw_sufficientFunds_succeeds() {
        BankAccountWithLock account = new BankAccountWithLock(100);
        assertTrue(account.withdraw(40));
        assertEquals(60, account.getBalance());
    }

    @Test
    void withdraw_insufficientFunds_returnsFalseWithoutChangingBalance() {
        BankAccountWithLock account = new BankAccountWithLock(50);
        assertFalse(account.withdraw(100));
        assertEquals(50, account.getBalance());
    }

    @Test
    void tryWithdraw_uncontended_succeedsQuickly() throws InterruptedException {
        BankAccountWithLock account = new BankAccountWithLock(100);
        assertTrue(account.tryWithdraw(30, 1, TimeUnit.SECONDS));
        assertEquals(70, account.getBalance());
    }

    @Test
    void concurrentDepositsAndWithdrawals_neverLoseAnUpdate() throws InterruptedException {
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

        assertEquals(1000.0, account.getBalance());
    }
}
