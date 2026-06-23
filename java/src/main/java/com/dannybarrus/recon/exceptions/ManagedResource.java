package com.dannybarrus.recon.exceptions;

import java.util.List;

/**
 * A minimal {@code AutoCloseable} that records every open and close event
 * into a shared trace list, so the ORDER of closing -- especially with
 * multiple resources -- is verifiable rather than something you just have
 * to trust from console output.
 *
 * <p>try-with-resources guarantees {@code close()} is called, even if an
 * exception is thrown inside the try block, and even with multiple
 * resources declared. Multiple resources are closed in the <b>reverse</b>
 * of the order they were opened, the same way stack unwinding works.
 */
public class ManagedResource implements AutoCloseable {

    private final String name;
    private final List<String> trace;

    public ManagedResource(String name, List<String> trace) {
        this.name = name;
        this.trace = trace;
        trace.add("opened " + name);
    }

    public void use() {
        trace.add("using " + name);
    }

    @Override
    public void close() {
        trace.add("closed " + name);
    }
}
