package com.dannybarrus.recon.collections;

import java.util.Objects;

/**
 * A simple value class implementing {@link Comparable} to define its
 * natural ordering -- by name, alphabetically.
 *
 * <p>Natural ordering is the ordering an object has "by default" when
 * sorted with no explicit comparator -- {@code Collections.sort(list)} or
 * {@code list.sort(null)} both use it. It should reflect the single most
 * obvious or commonly-expected ordering for the type. Anything else
 * (sort by salary, by department, by hire date) belongs in a
 * {@link java.util.Comparator}, not in {@code compareTo}.
 */
public final class Employee implements Comparable<Employee> {

    private final String name;
    private final String department;
    private final double salary;

    public Employee(String name, String department, double salary) {
        this.name = name;
        this.department = department;
        this.salary = salary;
    }

    public String getName() {
        return name;
    }

    public String getDepartment() {
        return department;
    }

    public double getSalary() {
        return salary;
    }

    @Override
    public int compareTo(Employee other) {
        // Natural ordering: by name. String.compareTo already returns the
        // negative/zero/positive convention compareTo is required to follow.
        return this.name.compareTo(other.name);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Employee)) return false;
        Employee that = (Employee) o;
        return Double.compare(salary, that.salary) == 0
            && Objects.equals(name, that.name)
            && Objects.equals(department, that.department);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name, department, salary);
    }

    @Override
    public String toString() {
        return name + " (" + department + ", $" + String.format("%,.0f", salary) + ")";
    }
}
