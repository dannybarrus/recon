package com.dannybarrus.recon.collections;

import java.util.Objects;

/**
 * A correctly implemented value class -- demonstrates the equals/hashCode
 * contract done right.
 *
 * <p>The contract, per {@link Object#equals(Object)} and
 * {@link Object#hashCode()}:
 * <ul>
 *   <li><b>Reflexive:</b> {@code x.equals(x)} is always true.</li>
 *   <li><b>Symmetric:</b> {@code x.equals(y)} iff {@code y.equals(x)}.</li>
 *   <li><b>Transitive:</b> if {@code x.equals(y)} and {@code y.equals(z)},
 *       then {@code x.equals(z)}.</li>
 *   <li><b>Consistent:</b> repeated calls return the same result, provided
 *       the objects are not mutated.</li>
 *   <li><b>equals(null)</b> must return false, never throw.</li>
 *   <li><b>Equal objects must have equal hashCodes.</b> This is the one
 *       most commonly violated -- see {@link BrokenPoint} for what happens
 *       when it is.</li>
 * </ul>
 *
 * <p>This class is immutable, which is the easiest way to satisfy
 * "consistent" -- there is nothing to mutate that would change the result.
 */
public final class Point {

    private final int x;
    private final int y;

    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;               // reflexive, and a cheap early exit
        }
        if (!(other instanceof Point)) {
            return false;               // covers null and other types -- never throws
        }
        Point that = (Point) other;
        return this.x == that.x && this.y == that.y;
    }

    @Override
    public int hashCode() {
        // Objects.hash combines fields the same way Point's equals compares
        // them. Equal Points are guaranteed to produce equal hash codes,
        // satisfying the contract.
        return Objects.hash(x, y);
    }

    @Override
    public String toString() {
        return "Point(" + x + ", " + y + ")";
    }
}
