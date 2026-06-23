package com.dannybarrus.recon.oop;

/**
 * Demonstrates what an abstract class offers that an interface cannot:
 * constructors, instance state, and a template method that relies on that
 * state plus an abstract step subclasses must fill in.
 *
 * <p>This is the Template Method pattern: {@code performSong()} defines a
 * fixed algorithm skeleton (tune, then play, then announce), but the
 * {@code tune()} step is abstract -- each subclass decides how tuning
 * actually works for its kind of instrument. The skeleton itself, and the
 * {@code name} state it depends on, live exactly once, here.
 *
 * <p>An interface could not hold {@code name} as instance state, and prior
 * to default methods, could not provide {@code performSong()}'s shared
 * implementation either -- this is the case for choosing an abstract class
 * when there is real shared state and a real shared algorithm, not just a
 * shared method signature.
 */
public abstract class Instrument {

    private final String name;
    private boolean tuned = false;

    protected Instrument(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    /** Each concrete instrument defines its own tuning behaviour. */
    protected abstract void tune();

    /**
     * The template method. The sequence (tune, then play) is fixed and
     * lives here exactly once -- subclasses cannot accidentally play
     * before tuning, because they never get to control the order.
     */
    public final void performSong() {
        tune();
        tuned = true;
        System.out.println(name + " is tuned. Performing now:");
        play();
    }

    /** The actual sound-producing step -- implemented via Playable, not here. */
    protected abstract void play();

    public boolean isTuned() {
        return tuned;
    }
}
