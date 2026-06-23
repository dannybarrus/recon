package com.dannybarrus.recon.exceptions;

/**
 * A custom <b>unchecked</b> exception -- extends {@code RuntimeException}.
 *
 * <p>Unchecked is the deliberate choice here: an invalid order (negative
 * quantity, blank product name) represents a caller passing bad data,
 * which is closer to a programming error than a recoverable business
 * condition. Forcing every caller up the stack to declare or catch this
 * would add noise for something that should simply never happen if the
 * caller is correct.
 */
public class InvalidOrderException extends RuntimeException {

    public InvalidOrderException(String message) {
        super(message);
    }
}
