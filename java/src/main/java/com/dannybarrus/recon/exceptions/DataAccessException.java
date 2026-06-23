package com.dannybarrus.recon.exceptions;

/**
 * Demonstrates exception chaining/wrapping: a higher-level, more
 * meaningful exception that preserves the original low-level cause via
 * the {@code (message, cause)} constructor inherited from
 * {@code Throwable}.
 *
 * <p>The point of wrapping rather than letting the low-level exception
 * propagate unchanged: callers of a data layer shouldn't need to know or
 * care that the underlying implementation happens to use, say,
 * {@code Integer.parseInt} on a flat file. They should see a
 * {@code DataAccessException} describing the failure in terms of what was
 * being attempted, with the original low-level exception still attached
 * via {@code getCause()} for anyone who needs to dig deeper.
 */
public class DataAccessException extends RuntimeException {

    public DataAccessException(String message, Throwable cause) {
        super(message, cause);
    }
}
