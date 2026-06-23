package com.dannybarrus.recon.collections;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class EmployeeTest {

    @Test
    void compareTo_ordersByNameAlphabetically() {
        Employee alice = new Employee("Alice", "Engineering", 100_000);
        Employee bob = new Employee("Bob", "Engineering", 100_000);
        assertTrue(alice.compareTo(bob) < 0);
    }

    @Test
    void naturalOrdering_sortsListByName() {
        List<Employee> list = new ArrayList<>(List.of(
            new Employee("Carol", "Eng", 1),
            new Employee("Alice", "Eng", 1),
            new Employee("Bob", "Eng", 1)
        ));
        list.sort(null);
        assertEquals("Alice", list.get(0).getName());
        assertEquals("Bob", list.get(1).getName());
        assertEquals("Carol", list.get(2).getName());
    }

    @Test
    void comparator_bySalary_sortsAscending() {
        List<Employee> list = new ArrayList<>(List.of(
            new Employee("A", "Eng", 300),
            new Employee("B", "Eng", 100),
            new Employee("C", "Eng", 200)
        ));
        list.sort(Comparator.comparingDouble(Employee::getSalary));
        assertEquals(100.0, list.get(0).getSalary());
        assertEquals(200.0, list.get(1).getSalary());
        assertEquals(300.0, list.get(2).getSalary());
    }

    @Test
    void comparator_chained_departmentThenName() {
        List<Employee> list = new ArrayList<>(List.of(
            new Employee("Bob", "Marketing", 1),
            new Employee("Alice", "Engineering", 1),
            new Employee("Carol", "Engineering", 1)
        ));
        list.sort(Comparator.comparing(Employee::getDepartment)
                            .thenComparing(Employee::getName));

        assertEquals("Alice", list.get(0).getName());     // Engineering, A
        assertEquals("Carol", list.get(1).getName());     // Engineering, C
        assertEquals("Bob", list.get(2).getName());       // Marketing
    }

    @Test
    void equals_sameFields_areEqual() {
        Employee a = new Employee("Alice", "Eng", 100_000);
        Employee b = new Employee("Alice", "Eng", 100_000);
        assertEquals(a, b);
        assertEquals(a.hashCode(), b.hashCode());
    }
}
