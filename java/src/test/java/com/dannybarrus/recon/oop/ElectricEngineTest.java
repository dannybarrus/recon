package com.dannybarrus.recon.oop;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class ElectricEngineTest {

    @Test
    void describe_includesBatteryCapacity() {
        ElectricEngine engine = new ElectricEngine(75);
        assertEquals("75kWh electric engine", engine.describe());
    }
}
