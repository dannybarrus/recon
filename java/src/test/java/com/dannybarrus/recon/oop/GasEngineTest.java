package com.dannybarrus.recon.oop;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class GasEngineTest {

    @Test
    void describe_includesHorsepower() {
        GasEngine engine = new GasEngine(300);
        assertEquals("300hp gasoline engine", engine.describe());
    }
}
