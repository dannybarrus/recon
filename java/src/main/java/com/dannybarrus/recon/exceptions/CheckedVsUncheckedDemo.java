package com.dannybarrus.recon.exceptions;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

/**
 * Demonstrates the difference between checked and unchecked exceptions.
 *
 * <p><b>Checked</b> -- subclasses of {@code Exception} (excluding
 * {@code RuntimeException}). The compiler forces every caller to either
 * catch it or declare it in their own {@code throws} clause; ignoring it
 * entirely is a compile error. {@code IOException} is the classic example
 * -- any I/O operation can fail, and Java wants that acknowledged in the
 * method signature.
 *
 * <p><b>Unchecked</b> -- subclasses of {@code RuntimeException}. The
 * compiler does not require callers to do anything about these.
 * {@code NumberFormatException}, {@code NullPointerException}, and
 * {@code IllegalArgumentException} are all unchecked -- Java's position is
 * that these usually represent programming errors rather than recoverable
 * conditions, so forcing every caller everywhere to handle them would add
 * noise without adding real safety.
 */
public final class CheckedVsUncheckedDemo {

    private CheckedVsUncheckedDemo() { }

    /** CHECKED -- callers must catch IOException or declare it themselves. */
    static String readFirstLine(String path) throws IOException {
        return Files.readString(Path.of(path)).lines().findFirst().orElse("");
    }

    /** UNCHECKED -- callers are free to ignore this entirely; the compiler will not stop them. */
    static int parseAge(String input) {
        return Integer.parseInt(input);
    }

    public static void main(String[] args) {
        demoChecked();
        System.out.println();
        demoUnchecked();
    }

    private static void demoChecked() {
        System.out.println("--- Checked exception: IOException ---");
        try {
            String line = readFirstLine("/this/path/does/not/exist.txt");
            System.out.println("Read: " + line);
        } catch (IOException e) {
            System.out.println("Caught IOException (forced by the compiler): " + e.getClass().getSimpleName());
        }
        System.out.println("Note: the try/catch above is not optional -- this method would not");
        System.out.println("compile without either catching IOException or declaring 'throws IOException'.");
    }

    private static void demoUnchecked() {
        System.out.println("--- Unchecked exception: NumberFormatException ---");

        System.out.println("parseAge(\"30\") -> " + parseAge("30"));

        try {
            parseAge("not-a-number");
        } catch (NumberFormatException e) {
            System.out.println("Caught NumberFormatException (by choice, not compiler requirement): "
                + e.getMessage());
        }
        System.out.println("Note: catching this is optional. The method compiles fine even");
        System.out.println("if no caller ever catches NumberFormatException at all.");
    }
}
