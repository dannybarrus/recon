# Recon — Java

A Maven-based Java reference library, recreated from four original NetBeans interview-prep projects (Anagrams, LonelyInteger, SinglyLinkedList, CyclesInALinkedList) and brought up to the same standard as the Go and C# parts of this repo: clean package structure, generics where the originals used raw `Object`, JUnit 5 tests, and documented fixes for real bugs found along the way.

## Why Maven

Unlike Go, where `testing` is part of the standard library, Java has no built-in test framework. JUnit is an external dependency, and Maven (or Gradle) is the standard way every professional Java codebase manages that dependency, compiles, and runs tests. A `pom.xml` that resolves cleanly and a `mvn test` that passes is itself a relevant signal for any role where Java is a listed core skill.

## Project Structure

```
java\
  pom.xml
  src\
    main\java\com\dannybarrus\recon\
      algorithms\
        Anagrams.java
        LonelyInteger.java
      datastructures\
        Node.java
        LinkedList.java
        LinkedListDemo.java
      collections\
        Point.java
        BrokenPoint.java
        EqualsAndHashCodeDemo.java
        Employee.java
        ComparableAndComparatorDemo.java
        CollectionPerformance.java
        ConcurrentModificationAndFixes.java
        ImmutableCollections.java
      streams\
        WordFrequency.java
        MethodReferencesDemo.java
        EmployeeCollectors.java
        ReduceExamples.java
        LazyEvaluation.java
        OptionalUsage.java
        ParallelStreamCaution.java
      oop\
        Playable.java
        Instrument.java
        Guitar.java
        InterfaceVsAbstractClassDemo.java
        Engine.java
        GasEngine.java
        ElectricEngine.java
        Car.java
        CompositionVsInheritanceDemo.java
        PolymorphismAndBindingDemo.java
        Swimmer.java
        Runner.java
        Triathlete.java
        DiamondProblemDemo.java
      exceptions\
        CheckedVsUncheckedDemo.java
        InsufficientFundsException.java
        BankAccount.java
        InvalidOrderException.java
        OrderValidator.java
        CustomExceptionsDemo.java
        ManagedResource.java
        TryWithResourcesDemo.java
        DataAccessException.java
        ExceptionChainingDemo.java
        FinallyGotchaDemo.java
        MultiCatchDemo.java
      concurrency\
        ThreadBasicsDemo.java
        UnsafeCounter.java
        SafeCounter.java
        RaceConditionDemo.java
        BankAccountWithLock.java
        LockDemo.java
        SimpleBoundedBuffer.java
        ProducerConsumerDemo.java
        TaskRunner.java
        ExecutorServiceDemo.java
        CompletableFutureExamples.java
        VolatileVisibilityDemo.java
        DeadlockDemo.java
    test\java\com\dannybarrus\recon\
      (mirrors main — one test class per testable main class; see "What's Here" below for which classes have tests)
```

## What's Here

### algorithms

| Class | Problem | Approach |
|---|---|---|
| `Anagrams` | Minimum character removals to make two strings anagrams | 26-element character histogram, sum of absolute differences |
| `LonelyInteger` | Find the one element that appears once in an array where everything else appears in pairs | XOR every element — duplicates cancel out, O(n) time, O(1) space |

### datastructures

| Class | Problem | Approach |
|---|---|---|
| `LinkedList<T>` | Singly linked list with basic operations and cycle detection | Explicit sentinel head node; Floyd's tortoise-and-hare for cycle detection |
| `Node<T>` | Backing node for `LinkedList<T>` | Generic data + next-pointer |
| `LinkedListDemo` | Runs both the basic-operations and cycle-detection demos | — (demo only) |

### collections

| Class | Topic | Approach |
|---|---|---|
| `Point` | Correct `equals()`/`hashCode()` contract | `Objects.hash()`, reflexive/symmetric/null-safe `equals()` |
| `BrokenPoint` | What breaks when `hashCode()` is omitted | `equals()` overridden, `hashCode()` left as identity-based default — `HashSet` fails to deduplicate |
| `EqualsAndHashCodeDemo` | Runs both side by side | — (demo only) |
| `Employee` | Natural ordering via `Comparable` | `compareTo()` by name |
| `ComparableAndComparatorDemo` | `Comparator.comparing()`, `.reversed()`, `.thenComparing()` chains | — (demo only) |
| `CollectionPerformance` | Empirical `ArrayList` vs `LinkedList` timing | Append, prepend, indexed-read benchmarks |
| `ConcurrentModificationAndFixes` | `ConcurrentModificationException` and three fixes | `Iterator.remove()`, `removeIf()`, iterate-a-copy |
| `ImmutableCollections` | `List.of()` vs `unmodifiableList()` vs defensive copy | Demonstrates the `unmodifiableList()`-still-mutable-via-backing-list gotcha |

### streams

| Class | Topic | Approach |
|---|---|---|
| `WordFrequency` | filter/map/collect pipeline | `Collectors.groupingBy(..., counting())`, top-N extraction |
| `MethodReferencesDemo` | All four kinds of method reference | Static, bound instance, unbound instance, constructor — each shown lambda-first |
| `EmployeeCollectors` | `Collectors` beyond `toList()` | `groupingBy`, `partitioningBy`, `toMap`, `joining`, `summarizingDouble` — reuses `Employee` from `collections` |
| `ReduceExamples` | `reduce()`/fold semantics | 2-arg and 3-arg `reduce`, ties to how `Collectors` are built |
| `LazyEvaluation` | Laziness and per-element pipelining | Execution trace proves filter/map interleave per-element, not pass-by-pass; short-circuiting with `findFirst()` |
| `OptionalUsage` | `Optional` done right vs wrong | `get()` blind vs `orElse`/`orElseGet`/`ifPresentOrElse`/`orElseThrow` |
| `ParallelStreamCaution` | Shared mutable state in `parallelStream()` | Broken counter vs `AtomicInteger` vs `count()` |

### oop

| Class | Topic | Approach |
|---|---|---|
| `Playable` | Interface with default/static/abstract methods | Java 8/9 default and static interface methods |
| `Instrument` | Abstract class with state + Template Method | Constructor state, `performSong()` template calling abstract `tune()` |
| `Guitar` | Combines both | Single `play()` satisfies both supertypes via access widening |
| `InterfaceVsAbstractClassDemo` | Runs the above together | — (demo only) |
| `Engine` / `GasEngine` / `ElectricEngine` | Composable behavior | Interface + swappable implementations |
| `Car` | Composition over inheritance | Holds an `Engine` reference, swappable at runtime via `setEngine()` |
| `CompositionVsInheritanceDemo` | Engine swap on one `Car` instance | — (demo only) |
| `PolymorphismAndBindingDemo` | Override vs overload, static vs dynamic binding | `Animal`/`Dog` nested classes; proves overload resolution uses declared type, not runtime type |
| `Swimmer` / `Runner` | Two interfaces with conflicting default `move()` | — |
| `Triathlete` | Resolves the diamond problem | `Swimmer.super.move()` + `Runner.super.move()` |
| `DiamondProblemDemo` | Runs the conflict and resolution | — (demo only) |

### exceptions

| Class | Topic | Approach |
|---|---|---|
| `CheckedVsUncheckedDemo` | Checked vs unchecked | `IOException` (checked) vs `NumberFormatException` (unchecked) |
| `InsufficientFundsException` | Custom checked exception | Extends `Exception`; deliberate choice for a recoverable business condition |
| `BankAccount` | Throws the checked exception | `withdraw()` declares `throws InsufficientFundsException` |
| `InvalidOrderException` | Custom unchecked exception | Extends `RuntimeException`; deliberate choice for a caller error |
| `OrderValidator` | Throws the unchecked exception | — |
| `CustomExceptionsDemo` | Runs both side by side | — (demo only) |
| `ManagedResource` | `AutoCloseable` with a recorded open/close trace | Proves the reverse-close-order guarantee, not just claims it |
| `TryWithResourcesDemo` | Single resource, multiple resources, resource + exception | — (demo only) |
| `DataAccessException` | Exception chaining, preserves cause | `(message, cause)` constructor |
| `ExceptionChainingDemo` | Right vs wrong: preserving vs discarding the cause | — (demo only) |
| `FinallyGotchaDemo` | `return` inside `finally` swallows an exception | Contrasts with try-with-resources' suppressed-exception mechanism |
| `MultiCatchDemo` | Multi-catch and catch-ordering rule | `NumberFormatException \| NullPointerException`; ordering shown as commented pseudo-code (a real compile error can't run) |

### concurrency

| Class | Topic | Approach |
|---|---|---|
| `ThreadBasicsDemo` | Three ways to create a thread; `start()` vs `run()` | Proves the gotcha via printed thread names, not just claims |
| `UnsafeCounter` | Broken: unsynchronized `count++` | — |
| `SafeCounter` | Fixed: `synchronized` | — |
| `RaceConditionDemo` | Lost-update race, two fixes | `synchronized` vs `AtomicInteger` |
| `BankAccountWithLock` | `ReentrantLock` with `tryLock()` timeout | Always unlocks in `finally`; package-private `holdLockAndRun()` for demo contention |
| `LockDemo` | Concurrent deposits/withdrawals; real timeout under contention | Holder thread + `tryWithdraw()` — proves ~500ms timeout, not full 2s hold |
| `SimpleBoundedBuffer` | Hand-rolled producer-consumer | `wait()`/`notifyAll()` in a `while` loop (not `if`) |
| `ProducerConsumerDemo` | Same problem solved with `BlockingQueue` | Side-by-side contrast |
| `TaskRunner` | Clean `ExecutorService` batch pattern | Always shuts down in `finally`, even on task failure |
| `ExecutorServiceDemo` | `execute()` vs `submit()` | — (demo only) |
| `CompletableFutureExamples` | Async chaining | `thenApply`, `thenCompose`, `thenCombine`, `exceptionally` |
| `VolatileVisibilityDemo` | Visibility vs atomicity — two different guarantees | Bounded, daemon-threaded; honest about reproducibility limits |
| `DeadlockDemo` | Real two-lock deadlock, then the fix | Daemon threads + bounded `join()`; consistent lock ordering as the fix |

Each package has its own demo classes with verified console output — every one was compiled and run with a real JDK before being committed, not just hand-traced.

**File count:** 59 main classes, 30 test classes, 89 files total across 7 packages.

| Package | Main | Test |
|---|---|---|
| `algorithms` | 2 | 2 |
| `datastructures` | 3 | 1 |
| `collections` | 8 | 6 |
| `streams` | 7 | 6 |
| `oop` | 14 | 5 |
| `exceptions` | 12 | 5 |
| `concurrency` | 13 | 5 |

## Recreated From Originals — What Changed and Why

The original four files worked, but going through them carefully surfaced a few real bugs and some design choices worth fixing. Same spirit as the concurrency debugging documented in the Go projects: the mistakes found along the way are as instructive as the final code.

### Static counter bug — `LinkedList`'s size field

The original declared `private static int counter`. A `static` field is shared across **every instance** of the class — so creating two `LinkedList` objects in the same program meant they silently corrupted each other's size count. Fixed by making it an instance field. This is a classic Java gotcha and a fair interview question in its own right: "what's wrong with this code" given a static field used for per-instance state.

### Off-by-one in `remove(index)`

The original checked `if (index < 1 || index > size())`, which rejected `index < 1` — silently blocking removal of the very first element (index 0). Fixed to `index < 0`.

### Unhandled NullPointerException in `add(data, index)`

The original threw an NPE when inserting into an empty list, masked by `@SuppressWarnings("null")` rather than actually handled. The explicit sentinel-head design in the rewrite eliminates the empty-list special case entirely — `add(data, 0)` on an empty list now works the same way as on any other list, no suppression needed.

### Direct field access instead of the setter

`createCylclicLink` (also a typo, fixed to `createCyclicLink`) used `srcNode.next = destNode` directly instead of the `setNext()` setter used everywhere else in the class — inconsistent encapsulation. Fixed to use the setter.

### Stale variable in the cycle-detection demo

The original demo captured `boolean isCyclic = thisList.hasCycle();` **before** creating the cycle, then printed that same stale value twice. The comment claimed "Expected: true" on the second print, but it would have printed `false` both times. Fixed in `LinkedListDemo` by calling `hasCycle()` again after the cycle is created — the printed output now actually reflects what's being demonstrated.

### Two duplicated Node/LinkedList pairs, consolidated into one

The original had two nearly identical `Node`/`LinkedList` implementations — one for the basic-operations demo, one for the cycle-detection demo. Consolidated into a single generic `LinkedList<T>` that supports both, with `LinkedListDemo` running both demonstrations against the same class.

### Generics instead of raw `Object`

The original `Node` and `LinkedList` used `Object` for the data field, which loses type safety and forces a cast on every read. Generified to `Node<T>` and `LinkedList<T>`.

### Removed blocking stdin read in `Anagrams`

The original `Anagrams.main()` blocked on `BufferedReader`/`System.in`, requiring console input to run at all. None of the other examples in this repo require interactive input — they all run standalone with hardcoded sample data, which also matters if you ever want an automated CI smoke test (a blocking `main()` hangs a headless build). Replaced with hardcoded examples; the core logic (`minRemovalsToAnagram`, `countCharacters`, `computeDelta`) is unchanged and fully testable.

## Java-Specific Idioms — Not Just a C# Translation

Most of this repo's C# material has a clean Java analog (interfaces, SOLID, async patterns, DI). A few things in this library are genuinely Java-specific — patterns and gotchas that don't map onto C# the same way, because the languages made different design decisions.

### Exception chaining: `getCause()` and `Throwable.getCause()` chains

`DataAccessException` (in `exceptions/`) wraps a low-level `NumberFormatException` while preserving it via `getCause()`. C# has the conceptually similar `InnerException`, but Java's exception model is built around this far more pervasively — checked exceptions force the question of "what do I do with this failure" at compile time, which has no C# equivalent at all. C# exceptions are all unchecked; there is no checked/unchecked distinction to design around. The entire `CheckedVsUncheckedDemo` and the deliberate choice of `Exception` vs `RuntimeException` for each custom exception class in this repo (`InsufficientFundsException` checked, `InvalidOrderException` unchecked) is a design decision with no direct C# parallel — in C#, that choice doesn't exist.

### Checked exceptions as part of the method signature

`BankAccount.withdraw(double) throws InsufficientFundsException` — the `throws` clause is checked by the compiler, not just documentation. Forgetting to catch or declare it is a compile error. C# has no equivalent; exception documentation there is convention (XML doc comments) rather than something `javac` enforces.

### Default methods and the diamond problem

`Swimmer`, `Runner`, and `Triathlete` (in `oop/`) demonstrate a conflict that is specifically a consequence of Java 8 adding default method bodies to interfaces — and the compiler's refusal to silently pick one default over another, requiring an explicit `InterfaceName.super.method()` resolution. C# 8 added default interface methods too, but the broader cultural emphasis on this exact diamond-conflict scenario as an interview topic is much stronger in Java, since Java's `interface` was pitched for decades as deliberately *not* supporting implementation inheritance, making default methods a notable reversal of that promise.

### Static vs dynamic binding for overloads vs overrides

`PolymorphismAndBindingDemo` shows overload resolution happening at compile time based on the *declared* type of a reference, while overriding happens at runtime based on the *actual* object. This binding distinction is asked about constantly in Java interviews specifically — C# has the same underlying mechanics, but method hiding via `new` versus `override` in C# is more explicit at the call site, so the trap is less commonly built into "gotcha" interview questions there.

### try-with-resources and suppressed exceptions

`TryWithResourcesDemo` and `FinallyGotchaDemo` show Java's `AutoCloseable` resource-closing guarantee, including the specific mechanism (`getSuppressed()`) for preserving a secondary failure during cleanup without discarding the primary one. C#'s `using` statement and `IDisposable` solve the same core problem, but Java's suppressed-exception mechanism for the case where *both* the body and the cleanup throw is a specific, additional piece of plumbing C#'s `using` doesn't have a named equivalent for.

### Threads as a first-class object, not a lightweight abstraction

`Thread` in Java maps directly onto an OS thread — there is no lighter-weight unit underneath it the way Go has goroutines (managed by the Go runtime, thousands can exist cheaply) or C# has `Task`/`async`-`await` (which can run on a thread pool without one thread per logical operation). `ThreadBasicsDemo`'s `start()` vs `run()` gotcha is specifically a consequence of `Thread` being a real, heavyweight object you explicitly construct and launch — there's no equivalent gotcha in Go, where you simply write `go someFunc()` and the runtime handles the rest, or in C#, where `Task.Run(...)` never has a "did I forget to actually start this" trap the way a raw `Thread` does.

### synchronized as a language keyword, not a library type

`SafeCounter`'s `synchronized` keyword is built into the Java language itself — any object can serve as a lock via `synchronized(obj)` or a `synchronized` method, with the JVM handling the monitor underneath. This is a different design point than both Go (no built-in mutual exclusion keyword at all — `sync.Mutex` is a library type you explicitly construct and call `Lock()`/`Unlock()` on, exactly like Java's `ReentrantLock`) and C# (`lock` is syntactic sugar over `Monitor.Enter`/`Monitor.Exit`, conceptually close to Java's `synchronized` but with no equivalent to `wait()`/`notifyAll()` built into every object the way Java's does).

### wait()/notifyAll() on every object

Every Java object inherits `wait()`, `notify()`, and `notifyAll()` from `Object` itself — `SimpleBoundedBuffer` uses these directly. This is genuinely unusual among mainstream languages: Go's `sync.Cond` and C#'s `Monitor.Wait`/`Monitor.Pulse` provide the same capability, but as separate, explicitly-constructed types you opt into, not something every single object carries by default. The while-loop-not-if rule for handling spurious wakeups documented in `SimpleBoundedBuffer` applies to all three languages' equivalents, but only Java bakes the mechanism into every object's base contract.

## Verification Note

Every main-source file here was compiled and run with a real JDK (`javac` / `java`, OpenJDK 21) before being handed off, and every demo's output was checked against its documented expected value. The JUnit test files use stable, standard JUnit 5 syntax, but **were not executed in the environment that wrote them** — Maven Central is unreachable from there. Run `mvn test` yourself the first time to confirm; if anything fails, it's worth a second look together rather than assuming the docs are right.

### A real bug caught during verification: `DeadlockDemo`

The first version of `DeadlockDemo` shared one pair of lock objects across both its "trigger the deadlock" section and its "fix with consistent lock ordering" section. The deadlocked threads from the first section are daemon threads that never die — by definition, a real deadlock means they hold their locks forever. The second section's threads then tried to acquire those same permanently-held locks and hung too, for a completely different reason than the one being taught. A bash-level `timeout 30` safety net wrapped around every concurrency demo run caught this immediately (exit code 124). Fixed by giving each demo section its own fresh lock objects — now documented directly in the class's javadoc.

### Concurrency demos and single-core reproducibility

Several `concurrency` and `streams` demos (`RaceConditionDemo`, `ParallelStreamCaution`, `VolatileVisibilityDemo`'s atomicity half) depend on genuine multi-core contention to show their broken behavior. The environment these were verified in has only one available CPU core, so the broken versions sometimes come back correct — not because the bug isn't real, but because there's no real parallelism on that machine to expose it. On a normal multi-core machine, expect the broken versions to misbehave at least some of the time. `VolatileVisibilityDemo`'s **visibility** half is the exception — it reproduced the stale-read bug reliably even in the single-core environment, since that one depends on JIT caching behavior rather than thread-count.

## Running It

### 1. Install the JDK

Download and install JDK 21 or later from https://adoptium.net/ or https://www.oracle.com/java/technologies/downloads/. Verify with:

```bat
java -version
javac -version
```

### 2. Install Maven

Download from https://maven.apache.org/download.cgi, add the `bin` directory to your PATH, then verify:

```bat
mvn -version
```

Maven resolves JUnit and all other dependencies automatically from Maven Central the first time you run any command below — no manual jar downloads needed.

### 3. Compile

From the `java\` directory (where `pom.xml` lives):

```bash
mvn compile
```

This downloads all dependencies (JUnit, the exec plugin) the first time, then compiles every class in `src\main\java`.

### 4. Run the full test suite

```bash
mvn test
```

### 5. Run a specific class

```bash
mvn exec:java -Dexec.mainClass="com.dannybarrus.recon.oop.PolymorphismAndBindingDemo"
```

Swap in the fully-qualified class name of whichever demo you want — `package.path.ClassName`, matching the folder structure exactly. A few more examples spanning every package:

```bash
mvn exec:java -Dexec.mainClass="com.dannybarrus.recon.algorithms.Anagrams"
mvn exec:java -Dexec.mainClass="com.dannybarrus.recon.datastructures.LinkedListDemo"
mvn exec:java -Dexec.mainClass="com.dannybarrus.recon.collections.ImmutableCollections"
mvn exec:java -Dexec.mainClass="com.dannybarrus.recon.streams.LazyEvaluation"
mvn exec:java -Dexec.mainClass="com.dannybarrus.recon.exceptions.FinallyGotchaDemo"
mvn exec:java -Dexec.mainClass="com.dannybarrus.recon.concurrency.DeadlockDemo"
```

Only classes with a `main()` method can be run this way — anything ending in `Demo`, plus a few standalone classes like `Anagrams`, `LonelyInteger`, and `CompletableFutureExamples` that combine logic and a demo in one file. Classes without `main()` (the supporting classes a demo exercises) aren't meant to be run directly — see them through their tests or through whichever `*Demo` class uses them.

### 6. Package into a jar (optional)

```bash
mvn package
```

## Windows Setup

Covered in [Running It](#running-it) above — both the JDK and Maven install steps apply identically on Windows. No corporate proxy configuration is needed unless your network specifically blocks Maven Central.

## Example Output

```
--- Basic operations ---
List:        [0][1][2][3][4][5]
size():      6
get(3):      3   (expected: 3)
remove(2):   true   (element '2' removed)
get(3):      4   (expected: 4, shifted left after removal)
size():      5
List again:  [0][1][3][4][5]

--- Cycle detection (Floyd's tortoise and hare) ---
List:        [0][1][2][3][4][5]
hasCycle():  false   (expected: false)
hasCycle():  true   (expected: true, after linking index 5 -> index 3)
```
