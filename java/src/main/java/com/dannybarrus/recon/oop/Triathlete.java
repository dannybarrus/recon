package com.dannybarrus.recon.oop;

/**
 * Implements two interfaces that each provide a conflicting default
 * implementation of the same method -- the "diamond problem" that Java's
 * single class inheritance is otherwise designed to avoid, reintroduced
 * through default methods.
 *
 * <p>Java's rule: if a class implements two interfaces that both provide
 * a default for the same method signature, the class <b>must</b> override
 * that method itself. The compiler will not guess which default you meant
 * -- this is a compile error, not a runtime ambiguity, which is exactly
 * the trade-off that makes default methods safe to add to existing
 * interfaces without silently breaking implementers in ambiguous ways.
 *
 * <p>Inside the override, {@code InterfaceName.super.methodName()} is the
 * syntax for explicitly invoking a specific interface's default
 * implementation -- something that has no equivalent for regular method
 * calls, since {@code super} alone only ever refers to a superclass, not
 * an interface.
 */
public class Triathlete implements Swimmer, Runner {

    @Override
    public String move() {
        return Swimmer.super.move() + ", then " + Runner.super.move();
    }
}
