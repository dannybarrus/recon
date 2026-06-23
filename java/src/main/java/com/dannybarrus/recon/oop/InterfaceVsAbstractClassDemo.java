package com.dannybarrus.recon.oop;

/**
 * Demonstrates the practical difference between an interface and an
 * abstract class by running both halves of {@link Guitar} -- the part it
 * gets from {@link Instrument} (abstract class) and the part it gets from
 * {@link Playable} (interface).
 */
public final class InterfaceVsAbstractClassDemo {

    private InterfaceVsAbstractClassDemo() { }

    public static void main(String[] args) {
        Guitar guitar = new Guitar("Stratocaster", 6);

        System.out.println("--- From the abstract class (Instrument): shared state + template method ---");
        System.out.println("isTuned() before performSong(): " + guitar.isTuned() + "   (expected: false)");
        guitar.performSong();
        System.out.println("isTuned() after performSong():  " + guitar.isTuned() + "   (expected: true)");

        System.out.println("\n--- From the interface (Playable): default method, inherited for free ---");
        guitar.pause();

        System.out.println("\n--- From the interface (Playable): static method, called on the interface itself ---");
        System.out.println("Playable.supportedFormats(): " + Playable.supportedFormats());
    }
}
