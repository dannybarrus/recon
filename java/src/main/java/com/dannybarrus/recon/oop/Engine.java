package com.dannybarrus.recon.oop;

/**
 * The behaviour a {@link Car} composes, rather than inherits.
 *
 * <p>See {@link Car}'s javadoc for why this is composition (Car HAS-A
 * Engine) rather than inheritance. The more realistic temptation this
 * guards against is GasCar/ElectricCar both extending some common Car
 * base class -- see the comment in {@link CompositionVsInheritanceDemo}
 * for why that path gets awkward fast.
 */
public interface Engine {

    void start();

    String describe();
}
