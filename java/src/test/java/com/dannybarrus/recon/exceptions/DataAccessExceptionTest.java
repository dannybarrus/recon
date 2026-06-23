package com.dannybarrus.recon.exceptions;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DataAccessExceptionTest {

    @Test
    void loadUserId_validInput_returnsParsedValue() {
        assertEquals(42, ExceptionChainingDemo.loadUserId("42"));
    }

    @Test
    void loadUserId_invalidInput_wrapsInDataAccessException() {
        DataAccessException ex = assertThrows(DataAccessException.class,
            () -> ExceptionChainingDemo.loadUserId("not-a-number"));
        assertTrue(ex.getMessage().contains("not-a-number"));
    }

    @Test
    void loadUserId_invalidInput_preservesOriginalCause() {
        DataAccessException ex = assertThrows(DataAccessException.class,
            () -> ExceptionChainingDemo.loadUserId("also-bad"));
        assertNotNull(ex.getCause());
        assertEquals(NumberFormatException.class, ex.getCause().getClass());
    }

    @Test
    void constructor_preservesMessageAndCause() {
        RuntimeException cause = new RuntimeException("root cause");
        DataAccessException ex = new DataAccessException("wrapped", cause);
        assertEquals("wrapped", ex.getMessage());
        assertSame(cause, ex.getCause());
    }
}
