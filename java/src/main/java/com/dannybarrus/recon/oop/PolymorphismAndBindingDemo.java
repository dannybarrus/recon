package com.dannybarrus.recon.oop;

/**
 * Demonstrates the single most common Java "what does this print" trick
 * question: overriding is resolved at <b>runtime</b> based on the actual
 * object, but overload resolution is decided at <b>compile time</b> based
 * on the <em>declared</em> (static) type of the reference -- and those two
 * facts together produce a result that surprises almost everyone the first
 * time they see it.
 *
 * <p>Animal and Dog are small, self-contained, and exist only for this
 * demonstration -- they live in this file rather than as separate classes
 * since neither has any reusable logic worth testing on its own.
 */
public final class PolymorphismAndBindingDemo {

    private PolymorphismAndBindingDemo() { }

    static class Animal {
        String makeSound() {
            return "Some generic animal sound";
        }
    }

    static class Dog extends Animal {
        @Override
        String makeSound() {
            return "Woof";
        }

        String fetch() {
            return "Fetching the ball";
        }
    }

    /** Overloaded on parameter type -- this is resolved at COMPILE time, not runtime. */
    static String identify(Animal a) {
        return "identify(Animal) called";
    }

    static String identify(Dog d) {
        return "identify(Dog) called";
    }

    public static void main(String[] args) {
        demoOverriding_resolvedAtRuntime();
        System.out.println();
        demoOverloading_resolvedAtCompileTime();
    }

    private static void demoOverriding_resolvedAtRuntime() {
        System.out.println("--- Overriding: resolved at RUNTIME, based on the actual object ---");

        Animal a = new Dog();

        System.out.println("Declared type: Animal.  Actual type: Dog.");
        System.out.println("a.makeSound() -> " + a.makeSound());
        System.out.println("(Calls Dog's override, not Animal's -- the JVM looks at the ACTUAL");
        System.out.println("object at runtime to decide which makeSound() to run. This is");
        System.out.println("dynamic dispatch, and it's what most people correctly expect.)");
    }

    private static void demoOverloading_resolvedAtCompileTime() {
        System.out.println("--- Overloading: resolved at COMPILE TIME, based on the DECLARED type ---");

        Animal a = new Dog();
        Dog d = new Dog();

        System.out.println("identify(a) -> " + identify(a) + "   (a is declared Animal)");
        System.out.println("identify(d) -> " + identify(d) + "   (d is declared Dog)");
        System.out.println();
        System.out.println("Both variables hold the exact same kind of object at runtime (a Dog),");
        System.out.println("but identify(a) calls the Animal overload anyway. The compiler decided");
        System.out.println("which overload to call using the DECLARED type of the variable, before");
        System.out.println("the program ever ran -- it never looks at what the object actually is.");
        System.out.println();
        System.out.println("This is the trap: overriding feels like it 'looks at the real object,'");
        System.out.println("so people assume overloading does too. It does not. Only one of these");
        System.out.println("mechanisms is dynamic.");
    }
}
