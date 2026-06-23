package com.dannybarrus.recon.oop;

/**
 * Demonstrates swapping a Car's engine at runtime -- the concrete payoff
 * of composing an {@link Engine} rather than inheriting from a fixed
 * gas/electric class hierarchy.
 */
public final class CompositionVsInheritanceDemo {

    private CompositionVsInheritanceDemo() { }

    public static void main(String[] args) {
        Car car = new Car("Roadster", new GasEngine(300));

        System.out.println("--- Starting with a gas engine ---");
        car.start();

        System.out.println("\n--- Swapping to an electric engine at runtime ---");
        System.out.println("(This would require constructing an entirely new object if");
        System.out.println("'engine type' were baked into the class hierarchy via inheritance.)");
        car.setEngine(new ElectricEngine(75));
        car.start();

        System.out.println("\n--- Same Car instance throughout ---");
        System.out.println("Model is still: " + car.getModel() + "   (unchanged -- only the engine swapped)");
    }
}
