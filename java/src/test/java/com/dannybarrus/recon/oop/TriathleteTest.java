package com.dannybarrus.recon.oop;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class TriathleteTest {

    @Test
    void move_combinesBothDefaultImplementations() {
        Triathlete triathlete = new Triathlete();
        assertEquals("swimming, then running", triathlete.move());
    }

    @Test
    void move_isDistinctFromEitherInterfaceAlone() {
        Triathlete triathlete = new Triathlete();
        String result = triathlete.move();

        assertTrue(result.contains("swimming"));
        assertTrue(result.contains("running"));
    }
}
