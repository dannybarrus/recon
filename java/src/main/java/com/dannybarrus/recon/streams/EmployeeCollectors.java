package com.dannybarrus.recon.streams;

import com.dannybarrus.recon.collections.Employee;

import java.util.DoubleSummaryStatistics;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * Demonstrates the {@code Collectors} most commonly reached for beyond
 * {@code toList()} -- grouping, partitioning, mapping to a different
 * collection shape, joining strings, and summary statistics.
 *
 * <p>Reuses {@link Employee} from the {@code collections} package rather
 * than declaring a parallel demo class -- the whole point of having built
 * a clean, tested domain class earlier is that it's reusable.
 */
public final class EmployeeCollectors {

    private EmployeeCollectors() { }

    /** Groups employees by department. Each department maps to its list of employees. */
    public static Map<String, List<Employee>> groupByDepartment(List<Employee> employees) {
        return employees.stream()
            .collect(Collectors.groupingBy(Employee::getDepartment));
    }

    /**
     * Partitions employees into two groups: those earning above the
     * threshold (key {@code true}) and those at or below it (key {@code false}).
     * Unlike groupingBy, partitioningBy always produces exactly two keys,
     * even if one group is empty.
     */
    public static Map<Boolean, List<Employee>> partitionBySalaryAbove(
            List<Employee> employees, double threshold) {
        return employees.stream()
            .collect(Collectors.partitioningBy(e -> e.getSalary() > threshold));
    }

    /** Builds a name-to-salary lookup map. Throws if two employees share a name. */
    public static Map<String, Double> nameToSalary(List<Employee> employees) {
        return employees.stream()
            .collect(Collectors.toMap(Employee::getName, Employee::getSalary));
    }

    /** Joins all employee names into a single comma-separated string. */
    public static String joinNames(List<Employee> employees) {
        return employees.stream()
            .map(Employee::getName)
            .collect(Collectors.joining(", "));
    }

    /** Computes count, sum, min, max, and average salary in a single pass. */
    public static DoubleSummaryStatistics salaryStatistics(List<Employee> employees) {
        return employees.stream()
            .collect(Collectors.summarizingDouble(Employee::getSalary));
    }

    /** Groups by department, then immediately reduces each group to its average salary. */
    public static Map<String, Double> averageSalaryByDepartment(List<Employee> employees) {
        return employees.stream()
            .collect(Collectors.groupingBy(
                Employee::getDepartment,
                Collectors.averagingDouble(Employee::getSalary)));
    }

    public static void main(String[] args) {
        List<Employee> employees = List.of(
            new Employee("Alice", "Engineering", 120_000),
            new Employee("Bob", "Marketing", 85_000),
            new Employee("Carol", "Engineering", 110_000),
            new Employee("Dave", "Marketing", 90_000),
            new Employee("Eve", "Engineering", 95_000)
        );

        System.out.println("--- groupingBy department ---");
        groupByDepartment(employees).forEach((dept, list) ->
            System.out.println("  " + dept + ": " + joinNames(list)));

        System.out.println("\n--- partitioningBy salary > 100000 ---");
        Map<Boolean, List<Employee>> partitioned = partitionBySalaryAbove(employees, 100_000);
        System.out.println("  Above:  " + joinNames(partitioned.get(true)));
        System.out.println("  At/below: " + joinNames(partitioned.get(false)));

        System.out.println("\n--- toMap name -> salary ---");
        nameToSalary(employees).forEach((name, salary) ->
            System.out.printf("  %-6s $%,.0f%n", name, salary));

        System.out.println("\n--- joining all names ---");
        System.out.println("  " + joinNames(employees));

        System.out.println("\n--- summary statistics ---");
        DoubleSummaryStatistics stats = salaryStatistics(employees);
        System.out.printf("  count=%d  sum=$%,.0f  min=$%,.0f  max=$%,.0f  avg=$%,.0f%n",
            stats.getCount(), stats.getSum(), stats.getMin(), stats.getMax(), stats.getAverage());

        System.out.println("\n--- average salary by department ---");
        averageSalaryByDepartment(employees).forEach((dept, avg) ->
            System.out.printf("  %-12s $%,.0f%n", dept, avg));
    }
}
