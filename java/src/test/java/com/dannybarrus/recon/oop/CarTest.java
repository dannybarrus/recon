package com.dannybarrus.recon.oop;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;

class CarTest {

    @Test
    void getEngine_returnsConstructorEngine() {
        Engine gas = new GasEngine(300);
        Car car = new Car("Roadster", gas);
        assertSame(gas, car.getEngine());
    }

    @Test
    void setEngine_swapsToNewEngine() {
        Car car = new Car("Roadster", new GasEngine(300));
        Engine electric = new ElectricEngine(75);

        car.setEngine(electric);

        assertSame(electric, car.getEngine());
    }

    @Test
    void getModel_unaffectedByEngineSwap() {
        Car car = new Car("Roadster", new GasEngine(300));
        car.setEngine(new ElectricEngine(75));
        assertEquals("Roadster", car.getModel());
    }
}
