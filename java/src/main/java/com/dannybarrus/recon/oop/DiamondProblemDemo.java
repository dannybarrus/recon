package com.dannybarrus.recon.oop;

/**
 * Demonstrates the diamond problem with default methods and its
 * resolution -- see {@link Triathlete} for the actual mechanics.
 */
public final class DiamondProblemDemo {

    private DiamondProblemDemo() { }

    public static void main(String[] args) {
        System.out.println("--- The diamond problem with default methods ---");
        System.out.println("Swimmer.move() default: \"swimming\"");
        System.out.println("Runner.move() default:  \"running\"");
        System.out.println();
        System.out.println("Triathlete implements both. Without an explicit override, this would");
        System.out.println("not even compile -- Java refuses to silently pick one default over");
        System.out.println("the other when there are two conflicting candidates.");
        System.out.println();

        Triathlete triathlete = new Triathlete();
        System.out.println("Triathlete.move() -> " + triathlete.move());
        System.out.println();
        System.out.println("Resolved explicitly inside the override using");
        System.out.println("Swimmer.super.move() and Runner.super.move() -- combining both");
        System.out.println("rather than arbitrarily discarding one.");
    }
}
