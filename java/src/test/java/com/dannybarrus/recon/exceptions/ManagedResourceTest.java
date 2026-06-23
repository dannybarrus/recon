package com.dannybarrus.recon.exceptions;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

class ManagedResourceTest {

    @Test
    void singleResource_opensAndClosesInOrder() {
        List<String> trace = new ArrayList<>();
        try (ManagedResource r = new ManagedResource("R", trace)) {
            r.use();
        }
        assertEquals(List.of("opened R", "using R", "closed R"), trace);
    }

    @Test
    void multipleResources_closeInReverseOrder() {
        List<String> trace = new ArrayList<>();
        try (ManagedResource a = new ManagedResource("A", trace);
             ManagedResource b = new ManagedResource("B", trace)) {
            a.use();
            b.use();
        }
        assertEquals(
            List.of("opened A", "opened B", "using A", "using B", "closed B", "closed A"),
            trace);
    }

    @Test
    void resourceClosesEvenWhenExceptionThrownInsideTry() {
        List<String> trace = new ArrayList<>();
        assertThrows(RuntimeException.class, () -> {
            try (ManagedResource r = new ManagedResource("R", trace)) {
                r.use();
                throw new RuntimeException("boom");
            }
        });
        assertTrue(trace.contains("closed R"));
    }
}
