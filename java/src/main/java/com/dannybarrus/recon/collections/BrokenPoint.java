package com.dannybarrus.recon.collections;

/**
 * A deliberately broken value class -- overrides {@code equals()} but not
 * {@code hashCode()}.
 *
 * <p>This is the single most common equals/hashCode mistake. Two
 * {@code BrokenPoint} instances that are logically equal will report
 * {@code equals() == true} but have different (identity-based) hash codes,
 * since the default {@link Object#hashCode()} is inherited unchanged.
 *
 * <p>The practical consequence: {@code HashSet} and {@code HashMap} use
 * {@code hashCode()} to decide which bucket to look in <em>before</em> they
 * ever call {@code equals()}. Two logically-equal BrokenPoints will almost
 * always land in different buckets, so the set will never recognise them
 * as duplicates -- even though {@code equals()} says they are equal.
 *
 * <p>See {@link EqualsAndHashCodeDemo} for the HashSet behaving incorrectly
 * because of this.
 */
public final class BrokenPoint {

    private final int x;
    private final int y;

    public BrokenPoint(int x, int y) {
        this.x = x;
        this.y = y;
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof BrokenPoint)) {
            return false;
        }
        BrokenPoint that = (BrokenPoint) other;
        return this.x == that.x && this.y == that.y;
    }

    // hashCode() intentionally NOT overridden -- this is the bug.
    // Object's default hashCode() is identity-based, so two BrokenPoints
    // with the same x and y will have different hash codes.

    @Override
    public String toString() {
        return "BrokenPoint(" + x + ", " + y + ")";
    }
}
