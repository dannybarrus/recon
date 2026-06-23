package com.dannybarrus.recon.streams;

import java.util.ArrayList;
import java.util.List;
import java.util.function.BiFunction;
import java.util.function.Function;
import java.util.function.Supplier;

/**
 * Demonstrates the four kinds of method reference in Java, each shown as a
 * lambda first, then the equivalent (and preferred, once the lambda is
 * trivial) method reference.
 *
 * <ol>
 *   <li><b>Static method:</b> {@code ClassName::staticMethod}</li>
 *   <li><b>Bound instance method:</b> {@code particularObject::instanceMethod}
 *       -- the receiver is already fixed at the time the reference is created.</li>
 *   <li><b>Unbound instance method:</b> {@code ClassName::instanceMethod}
 *       -- the receiver becomes the first parameter of the functional
 *       interface; used when you want to apply the same method across many
 *       different objects of that type.</li>
 *   <li><b>Constructor reference:</b> {@code ClassName::new}</li>
 * </ol>
 *
 * <p>Rule of thumb for when a method reference reads more clearly than a
 * lambda: if the lambda body is <em>only</em> a single method call with no
 * other logic, a method reference says the same thing with less syntax.
 * If there's any additional logic in the lambda body, keep the lambda --
 * forcing a method reference there usually requires an extra wrapper
 * method, which is worse, not better.
 */
public final class MethodReferencesDemo {

    private MethodReferencesDemo() { }

    public static void main(String[] args) {
        demoStaticMethodReference();
        demoBoundInstanceMethodReference();
        demoUnboundInstanceMethodReference();
        demoConstructorReference();
    }

    private static void demoStaticMethodReference() {
        System.out.println("--- 1. Static method reference ---");

        BiFunction<Integer, Integer, Integer> maxLambda = (a, b) -> Math.max(a, b);
        BiFunction<Integer, Integer, Integer> maxRef = Math::max;

        System.out.println("Lambda:           " + maxLambda.apply(3, 7));
        System.out.println("Method reference:  " + maxRef.apply(3, 7));
    }

    private static void demoBoundInstanceMethodReference() {
        System.out.println("\n--- 2. Bound instance method reference ---");
        // "bound" means the receiver (this particular String) is fixed
        // at the moment the reference is created.

        String greeting = "hello";

        Supplier<String> upperLambda = () -> greeting.toUpperCase();
        Supplier<String> upperRef = greeting::toUpperCase;

        System.out.println("Lambda:           " + upperLambda.get());
        System.out.println("Method reference:  " + upperRef.get());
    }

    private static void demoUnboundInstanceMethodReference() {
        System.out.println("\n--- 3. Unbound instance method reference ---");
        // "unbound" means there is no particular receiver yet -- the
        // receiver becomes the first argument to the functional interface.
        // String::toUpperCase here means "call toUpperCase() on whatever
        // String is passed in", not on any specific string.

        Function<String, String> upperLambda = s -> s.toUpperCase();
        Function<String, String> upperRef = String::toUpperCase;

        System.out.println("Lambda:           " + upperLambda.apply("world"));
        System.out.println("Method reference:  " + upperRef.apply("world"));
    }

    private static void demoConstructorReference() {
        System.out.println("\n--- 4. Constructor reference ---");

        Supplier<List<String>> listLambda = () -> new ArrayList<>();
        Supplier<List<String>> listRef = ArrayList::new;

        System.out.println("Lambda creates:           " + listLambda.get().getClass().getSimpleName());
        System.out.println("Method reference creates: " + listRef.get().getClass().getSimpleName());
    }
}
