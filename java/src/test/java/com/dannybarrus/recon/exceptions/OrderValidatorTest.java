package com.dannybarrus.recon.exceptions;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

class OrderValidatorTest {

    @Test
    void validate_validOrder_doesNotThrow() {
        assertDoesNotThrow(() -> OrderValidator.validate("Widget", 5));
    }

    @Test
    void validate_blankProductName_throws() {
        assertThrows(InvalidOrderException.class, () -> OrderValidator.validate("", 5));
    }

    @Test
    void validate_nullProductName_throws() {
        assertThrows(InvalidOrderException.class, () -> OrderValidator.validate(null, 5));
    }

    @Test
    void validate_zeroQuantity_throws() {
        assertThrows(InvalidOrderException.class, () -> OrderValidator.validate("Widget", 0));
    }

    @Test
    void validate_negativeQuantity_throws() {
        assertThrows(InvalidOrderException.class, () -> OrderValidator.validate("Widget", -3));
    }

    @Test
    void invalidOrderException_isUnchecked() {
        assertTrue(RuntimeException.class.isAssignableFrom(InvalidOrderException.class));
    }
}
