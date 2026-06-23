package com.dannybarrus.recon.oop;

/**
 * Combines both: extends the abstract class {@link Instrument} (for shared
 * state and the template method) and implements the interface
 * {@link Playable} (for the pluggable behaviour contract).
 *
 * <p>Note that {@code play()} is required by both supertypes, but Guitar
 * only needs to write it once -- a single {@code public void play()}
 * satisfies Instrument's {@code protected abstract void play()} (widening
 * access from protected to public is always allowed when overriding) and
 * Playable's {@code void play()} (implicitly public, as all interface
 * methods are) at the same time.
 *
 * <p>This is also the answer to "why not just use one or the other": a
 * class can extend only one class (single inheritance), but can implement
 * as many interfaces as it needs. Guitar gets its single source of shared
 * state and algorithm from one abstract class, and is free to also pick up
 * any number of additional behavioural contracts via interfaces.
 */
public class Guitar extends Instrument implements Playable {

    private final int stringCount;

    public Guitar(String name, int stringCount) {
        super(name);
        this.stringCount = stringCount;
    }

    @Override
    protected void tune() {
        System.out.println("Tuning all " + stringCount + " strings of the " + getName() + "...");
    }

    @Override
    public void play() {
        System.out.println("Strumming the " + getName() + ".");
    }

    // pause() is NOT overridden -- Guitar gets Playable's default behaviour for free.
    // supportedFormats() is not overridden either -- it's static, and static
    // interface methods are never inherited or overridden by implementers at all.
}
