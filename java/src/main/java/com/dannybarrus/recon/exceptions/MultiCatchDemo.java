package com.dannybarrus.recon.exceptions;

/**
 * Demonstrates multi-catch (Java 7+) and the catch-block ordering rule.
 */
public final class MultiCatchDemo {

    private MultiCatchDemo() { }

    static void process(String input, boolean simulateNumberError) {
        if (simulateNumberError) {
            Integer.parseInt(input);
        } else if (input == null) {
            throw new NullPointerException("input was null");
        }
    }

    public static void main(String[] args) {
        demoMultiCatch();
        System.out.println();
        demoCatchOrderingRule();
    }

    private static void demoMultiCatch() {
        System.out.println("--- Multi-catch: one block handling two unrelated exception types ---");

        for (boolean simulateNumberError : new boolean[] { true, false }) {
            try {
                process(simulateNumberError ? "not-a-number" : null, simulateNumberError);
            } catch (NumberFormatException | NullPointerException e) {
                System.out.println("  Caught " + e.getClass().getSimpleName() + ": " + e.getMessage());
            }
        }
    }

    private static void demoCatchOrderingRule() {
        System.out.println("--- Catch ordering: most specific first, most general last ---");
        System.out.println();
        System.out.println("  This is a COMPILE-TIME rule, not something a running demo can show --");
        System.out.println("  the invalid ordering below does not compile at all:");
        System.out.println();
        System.out.println("    try {");
        System.out.println("        Integer.parseInt(\"bad\");");
        System.out.println("    } catch (Exception e) {");
        System.out.println("        // handle generally");
        System.out.println("    } catch (NumberFormatException e) {  // COMPILE ERROR:");
        System.out.println("        // unreachable -- Exception above already catches this");
        System.out.println("    }");
        System.out.println();
        System.out.println("  The fix is to order from most specific to most general:");
        System.out.println();
        System.out.println("    try {");
        System.out.println("        Integer.parseInt(\"bad\");");
        System.out.println("    } catch (NumberFormatException e) {");
        System.out.println("        // handle the specific case");
        System.out.println("    } catch (Exception e) {");
        System.out.println("        // handle anything else");
        System.out.println("    }");
    }
}
