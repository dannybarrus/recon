package com.dannybarrus.recon.oop;

/**
 * Demonstrates what an interface can contain as of Java 8 and 9 -- not
 * just abstract method signatures.
 *
 * <ul>
 *   <li><b>Abstract method</b> {@code play()} -- the contract every
 *       implementer must fulfill. No body, no implementation choice.</li>
 *   <li><b>Default method</b> {@code pause()} (Java 8) -- a real method
 *       body. Implementers inherit this behaviour for free and may
 *       override it, but are not required to.</li>
 *   <li><b>Static method</b> {@code supportedFormats()} (Java 8) -- a
 *       utility method that belongs conceptually to the interface itself,
 *       not to any particular implementer. Called as
 *       {@code Playable.supportedFormats()}, never through an instance.</li>
 * </ul>
 *
 * <p>This is the modern answer to "why would you ever need a default
 * method instead of just an abstract class": it lets an interface evolve
 * (adding new behaviour to an existing contract) without breaking every
 * class that already implements it.
 */
public interface Playable {

    void play();

    default void pause() {
        System.out.println("Paused (default behaviour from Playable -- not overridden).");
    }

    static String supportedFormats() {
        return "MP3, WAV, FLAC";
    }
}
