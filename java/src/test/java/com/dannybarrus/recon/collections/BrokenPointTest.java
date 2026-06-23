package com.dannybarrus.recon.collections;

import org.junit.jupiter.api.Test;

import java.util.HashSet;
import java.util.Set;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

/**
 * Verifies BrokenPoint actually exhibits the bug it's named for -- this is
 * intentionally a test of broken behaviour, to make sure the demo class
 * keeps demonstrating what it claims to.
 */
class BrokenPointTest {

    @Test
    void equals_stillWorksCorrectly() {
        // The equals() override itself is fine -- only hashCode is missing.
        assertTrue(new BrokenPoint(1, 2).equals(new BrokenPoint(1, 2)));
    }

    @Test
    void hashSet_failsToDeduplicate_demonstratingTheBug() {
        Set<BrokenPoint> set = new HashSet<>();
        set.add(new BrokenPoint(1, 2));
        set.add(new BrokenPoint(1, 2));   // logically equal, but...

        // This assertion documents the bug rather than hiding it: a correct
        // implementation would have size 1, exactly like PointTest does.
        // If this ever starts failing because someone "fixes" BrokenPoint,
        // that's a sign the class no longer demonstrates what it's for.
        assertEquals(2, set.size());
    }
}
