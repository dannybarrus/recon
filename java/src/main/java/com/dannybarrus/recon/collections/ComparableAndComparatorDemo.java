package com.dannybarrus.recon.collections;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

/**
 * Demonstrates the difference between {@link Comparable} (natural ordering,
 * defined once, on the class itself) and {@link Comparator} (any number of
 * alternative orderings, defined externally, without modifying the class).
 *
 * <p>Rule of thumb: a class has at most one natural ordering ({@code
 * compareTo}), but as many useful comparators as you need for different
 * sorting contexts.
 */
public final class ComparableAndComparatorDemo {

    private ComparableAndComparatorDemo() { }

    public static void main(String[] args) {
        List<Employee> employees = sampleEmployees();

        System.out.println("--- Natural ordering (Comparable -- by name) ---");
        List<Employee> byName = new ArrayList<>(employees);
        byName.sort(null);   // null comparator -- uses compareTo
        byName.forEach(System.out::println);

        System.out.println("\n--- Comparator.comparing(salary) ---");
        List<Employee> bySalary = new ArrayList<>(employees);
        bySalary.sort(Comparator.comparingDouble(Employee::getSalary));
        bySalary.forEach(System.out::println);

        System.out.println("\n--- Comparator.comparing(salary).reversed() ---");
        List<Employee> bySalaryDesc = new ArrayList<>(employees);
        bySalaryDesc.sort(Comparator.comparingDouble(Employee::getSalary).reversed());
        bySalaryDesc.forEach(System.out::println);

        System.out.println("\n--- Chained: department, then name within department ---");
        List<Employee> byDeptThenName = new ArrayList<>(employees);
        byDeptThenName.sort(
            Comparator.comparing(Employee::getDepartment)
                      .thenComparing(Employee::getName));
        byDeptThenName.forEach(System.out::println);
    }

    private static List<Employee> sampleEmployees() {
        List<Employee> list = new ArrayList<>();
        list.add(new Employee("Carol", "Engineering", 110_000));
        list.add(new Employee("Alice", "Engineering", 120_000));
        list.add(new Employee("Bob", "Marketing", 85_000));
        list.add(new Employee("Dave", "Marketing", 90_000));
        return list;
    }
}
