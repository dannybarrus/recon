package com.dannybarrus.recon.collections;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

class ImmutableCollectionsTest {

    @Test
    void defensiveImmutableCopy_containsSameElements() {
        List<String> source = new ArrayList<>(List.of("a", "b", "c"));
        List<String> copy = ImmutableCollections.defensiveImmutableCopy(source);
        assertEquals(source, copy);
    }

    @Test
    void defensiveImmutableCopy_isUnmodifiable() {
        List<String> source = new ArrayList<>(List.of("a", "b", "c"));
        List<String> copy = ImmutableCollections.defensiveImmutableCopy(source);
        assertThrows(UnsupportedOperationException.class, () -> copy.add("d"));
    }

    @Test
    void defensiveImmutableCopy_isNotAffectedBySourceMutation() {
        // This is exactly the gotcha that Collections.unmodifiableList does
        // NOT protect against -- List.copyOf() does, because it copies the
        // data rather than wrapping the original list.
        List<String> source = new ArrayList<>(List.of("a", "b", "c"));
        List<String> copy = ImmutableCollections.defensiveImmutableCopy(source);

        source.add("z");

        assertEquals(List.of("a", "b", "c"), copy);   // unchanged
        assertEquals(List.of("a", "b", "c", "z"), source);
    }
}
