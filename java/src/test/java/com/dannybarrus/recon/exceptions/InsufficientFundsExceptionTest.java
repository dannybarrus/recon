package com.dannybarrus.recon.exceptions;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class InsufficientFundsExceptionTest {

    @Test
    void messageIncludesBothAmounts() {
        InsufficientFundsException ex = new InsufficientFundsException(100.0, 50.0);
        assertTrue(ex.getMessage().contains("100.00"));
        assertTrue(ex.getMessage().contains("50.00"));
    }

    @Test
    void gettersReturnConstructorValues() {
        InsufficientFundsException ex = new InsufficientFundsException(75.5, 20.25);
        assertEquals(75.5, ex.getRequestedAmount());
        assertEquals(20.25, ex.getAvailableBalance());
    }

    @Test
    void isACheckedException() {
        assertTrue(Exception.class.isAssignableFrom(InsufficientFundsException.class));
        assertFalse(RuntimeException.class.isAssignableFrom(InsufficientFundsException.class));
    }
}
