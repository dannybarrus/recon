package com.dannybarrus.recon.exceptions;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class BankAccountTest {

    @Test
    void deposit_increasesBalance() {
        BankAccount account = new BankAccount(100);
        account.deposit(50);
        assertEquals(150, account.getBalance());
    }

    @Test
    void deposit_negativeAmount_throwsIllegalArgumentException() {
        BankAccount account = new BankAccount(100);
        assertThrows(IllegalArgumentException.class, () -> account.deposit(-10));
    }

    @Test
    void withdraw_sufficientFunds_decreasesBalance() throws InsufficientFundsException {
        BankAccount account = new BankAccount(100);
        account.withdraw(40);
        assertEquals(60, account.getBalance());
    }

    @Test
    void withdraw_insufficientFunds_throwsCheckedException() {
        BankAccount account = new BankAccount(50);
        InsufficientFundsException ex = assertThrows(InsufficientFundsException.class,
            () -> account.withdraw(100));
        assertEquals(100.0, ex.getRequestedAmount());
        assertEquals(50.0, ex.getAvailableBalance());
    }

    @Test
    void withdraw_negativeAmount_throwsIllegalArgumentException() {
        BankAccount account = new BankAccount(100);
        assertThrows(IllegalArgumentException.class, () -> account.withdraw(-5));
    }
}
