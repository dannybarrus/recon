package com.dannybarrus.recon.exceptions;

import java.util.ArrayList;
import java.util.List;

public final class TryWithResourcesDemo {

    private TryWithResourcesDemo() { }

    public static void main(String[] args) {
        demoSingleResource();
        System.out.println();
        demoMultipleResourcesCloseInReverseOrder();
        System.out.println();
        demoResourceClosedEvenOnException();
    }

    private static void demoSingleResource() {
        System.out.println("--- Single resource, closed automatically ---");
        List<String> trace = new ArrayList<>();
        try (ManagedResource resource = new ManagedResource("FileHandle", trace)) {
            resource.use();
        }
        trace.forEach(line -> System.out.println("  " + line));
    }

    private static void demoMultipleResourcesCloseInReverseOrder() {
        System.out.println("--- Multiple resources, closed in REVERSE order of opening ---");
        List<String> trace = new ArrayList<>();
        try (ManagedResource a = new ManagedResource("A", trace);
             ManagedResource b = new ManagedResource("B", trace);
             ManagedResource c = new ManagedResource("C", trace)) {
            a.use();
            b.use();
            c.use();
        }
        trace.forEach(line -> System.out.println("  " + line));
        System.out.println("  Note: opened A, B, C -- closed C, B, A.");
    }

    private static void demoResourceClosedEvenOnException() {
        System.out.println("--- Resource still closes even when the try block throws ---");
        List<String> trace = new ArrayList<>();
        try {
            try (ManagedResource resource = new ManagedResource("RiskyResource", trace)) {
                resource.use();
                throw new RuntimeException("Something went wrong mid-use");
            }
        } catch (RuntimeException e) {
            trace.add("caught: " + e.getMessage());
        }
        trace.forEach(line -> System.out.println("  " + line));
        System.out.println("  Note: 'closed RiskyResource' appears BEFORE the catch ran --");
        System.out.println("  close() happens as the exception propagates out, not after it's handled.");
    }
}
