package com.dannybarrus.recon.concurrency;

/**
 * Demonstrates a real, classic two-lock deadlock: two threads each acquire
 * two locks, but in OPPOSITE order. Thread 1 locks A then tries to lock B;
 * Thread 2 locks B then tries to lock A. If timing interleaves badly, each
 * thread ends up holding the lock the other one needs -- neither can ever
 * proceed.
 *
 * <p>Both threads here are marked as daemon threads specifically so that
 * if the deadlock actually occurs, the JVM can still exit cleanly when
 * {@code main()} finishes, rather than hanging the whole program forever.
 * {@code main()} uses a bounded {@code join()} with a timeout to detect
 * whether the deadlock happened, rather than waiting forever to find out.
 *
 * <p><b>Important:</b> the deadlock-demonstration threads, once truly
 * deadlocked, never release their locks -- they are stuck forever by
 * definition. The two demo sections below therefore use entirely separate
 * lock objects, created fresh for each section. Reusing the same lock
 * objects across both would mean the "fix" demo's threads try to acquire
 * locks permanently held by the first demo's abandoned, still-deadlocked
 * threads -- which would hang the second demo too, for an entirely
 * different and unintended reason.
 */
public final class DeadlockDemo {

    private DeadlockDemo() { }

    public static void main(String[] args) throws InterruptedException {
        demoTheDeadlock();
        System.out.println();
        demoTheFix_consistentLockOrdering();
    }

    private static void demoTheDeadlock() throws InterruptedException {
        System.out.println("--- Triggering a real deadlock (bounded wait, daemon threads) ---");

        Object lockA = new Object();
        Object lockB = new Object();

        Thread t1 = new Thread(() -> {
            synchronized (lockA) {
                sleepQuietly(100);
                synchronized (lockB) {
                    System.out.println("  Thread 1 acquired both locks (no deadlock this run)");
                }
            }
        });

        Thread t2 = new Thread(() -> {
            synchronized (lockB) {
                sleepQuietly(100);
                synchronized (lockA) {
                    System.out.println("  Thread 2 acquired both locks (no deadlock this run)");
                }
            }
        });

        t1.setDaemon(true);
        t2.setDaemon(true);
        t1.start();
        t2.start();

        t1.join(2000);
        t2.join(2000);

        if (t1.isAlive() || t2.isAlive()) {
            System.out.println("  DEADLOCK CONFIRMED: at least one thread did not finish within 2 seconds.");
            System.out.println("  Thread 1 alive: " + t1.isAlive() + "   Thread 2 alive: " + t2.isAlive());
            System.out.println("  Both threads are daemons, so the program can still exit normally --");
            System.out.println("  in a real application, this would hang those two threads forever.");
            System.out.println("  (These threads are now abandoned, still holding their locks forever --");
            System.out.println("  the fix demo below uses entirely separate lock objects for that reason.)");
        } else {
            System.out.println("  No deadlock this run -- timing-dependent bugs don't trigger every time.");
            System.out.println("  That unpredictability is exactly what makes them so dangerous in production.");
        }
    }

    private static void demoTheFix_consistentLockOrdering() throws InterruptedException {
        System.out.println("--- The fix: both threads acquire locks in the SAME order ---");

        // Fresh lock objects -- never touched by the (possibly still
        // deadlocked) threads from the demo above.
        Object lockA = new Object();
        Object lockB = new Object();

        Thread t1 = new Thread(() -> {
            synchronized (lockA) {
                sleepQuietly(50);
                synchronized (lockB) {
                    System.out.println("  Thread 1 acquired both locks successfully.");
                }
            }
        });

        Thread t2 = new Thread(() -> {
            synchronized (lockA) {   // same order as t1: A then B, never B then A
                sleepQuietly(50);
                synchronized (lockB) {
                    System.out.println("  Thread 2 acquired both locks successfully.");
                }
            }
        });

        t1.start();
        t2.start();
        t1.join();   // safe without a timeout -- this version is provably deadlock-free
        t2.join();

        System.out.println("  Both threads finished. Consistent lock ordering makes the");
        System.out.println("  circular-wait condition that causes deadlock impossible.");
    }

    private static void sleepQuietly(long millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException ignored) { }
    }
}
