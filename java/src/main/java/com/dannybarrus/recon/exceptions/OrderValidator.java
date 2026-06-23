package com.dannybarrus.recon.exceptions;

public final class OrderValidator {

    private OrderValidator() { }

    public static void validate(String productName, int quantity) {
        if (productName == null || productName.isBlank()) {
            throw new InvalidOrderException("Product name must not be blank");
        }
        if (quantity <= 0) {
            throw new InvalidOrderException("Quantity must be positive, was: " + quantity);
        }
    }
}
