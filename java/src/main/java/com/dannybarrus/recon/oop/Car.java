package com.dannybarrus.recon.oop;

/**
 * Composes an {@link Engine} rather than inheriting from a gas- or
 * electric-specific base class.
 *
 * <p>The inheritance alternative would look tempting at first: a
 * {@code Car} base class, with {@code GasCar} and {@code ElectricCar}
 * subclasses. It falls apart the moment a hybrid exists -- Java has no
 * multiple class inheritance, so a {@code HybridCar} cannot extend both
 * {@code GasCar} and {@code ElectricCar}. Worse, switching a car's engine
 * after construction (a real thing -- engine swaps, EV conversions) is
 * impossible if "being gas-powered" is baked into the class hierarchy
 * itself rather than held as a replaceable field.
 *
 * <p>With composition, none of that is a problem. {@code Car} holds a
 * reference to whatever {@code Engine} it currently has, and that
 * reference can be swapped at runtime via {@link #setEngine}. A
 * "hybrid" doesn't need special-case inheritance at all -- it's just
 * another class implementing {@code Engine} that happens to delegate to
 * two engines internally, completely invisible to {@code Car}.
 */
public class Car {

    private final String model;
    private Engine engine;

    public Car(String model, Engine engine) {
        this.model = model;
        this.engine = engine;
    }

    /** Swaps the engine at runtime -- impossible to express this cleanly with inheritance. */
    public void setEngine(Engine engine) {
        this.engine = engine;
    }

    public void start() {
        System.out.print(model + " (" + engine.describe() + "): ");
        engine.start();
    }

    public String getModel() {
        return model;
    }

    public Engine getEngine() {
        return engine;
    }
}
