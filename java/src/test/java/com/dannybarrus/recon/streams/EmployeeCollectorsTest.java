package com.dannybarrus.recon.streams;

import com.dannybarrus.recon.collections.Employee;
import org.junit.jupiter.api.Test;

import java.util.DoubleSummaryStatistics;
import java.util.List;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class EmployeeCollectorsTest {

    private static List<Employee> sample() {
        return List.of(
            new Employee("Alice", "Engineering", 120_000),
            new Employee("Bob", "Marketing", 85_000),
            new Employee("Carol", "Engineering", 110_000),
            new Employee("Dave", "Marketing", 90_000)
        );
    }

    @Test
    void groupByDepartment_groupsCorrectly() {
        Map<String, List<Employee>> grouped = EmployeeCollectors.groupByDepartment(sample());
        assertEquals(2, grouped.get("Engineering").size());
        assertEquals(2, grouped.get("Marketing").size());
    }

    @Test
    void partitionBySalaryAbove_splitsCorrectly() {
        Map<Boolean, List<Employee>> partitioned =
            EmployeeCollectors.partitionBySalaryAbove(sample(), 100_000);

        assertEquals(2, partitioned.get(true).size());
        assertEquals(2, partitioned.get(false).size());
    }

    @Test
    void partitionBySalaryAbove_alwaysHasBothKeys_evenIfEmpty() {
        Map<Boolean, List<Employee>> partitioned =
            EmployeeCollectors.partitionBySalaryAbove(sample(), 1_000_000);

        assertTrue(partitioned.containsKey(true));
        assertEquals(0, partitioned.get(true).size());
        assertEquals(4, partitioned.get(false).size());
    }

    @Test
    void nameToSalary_buildsCorrectMap() {
        Map<String, Double> map = EmployeeCollectors.nameToSalary(sample());
        assertEquals(120_000.0, map.get("Alice"));
        assertEquals(85_000.0, map.get("Bob"));
    }

    @Test
    void joinNames_joinsWithCommaSpace() {
        assertEquals("Alice, Bob, Carol, Dave", EmployeeCollectors.joinNames(sample()));
    }

    @Test
    void joinNames_emptyList_returnsEmptyString() {
        assertEquals("", EmployeeCollectors.joinNames(List.of()));
    }

    @Test
    void salaryStatistics_computesCorrectly() {
        DoubleSummaryStatistics stats = EmployeeCollectors.salaryStatistics(sample());
        assertEquals(4, stats.getCount());
        assertEquals(405_000.0, stats.getSum());
        assertEquals(85_000.0, stats.getMin());
        assertEquals(120_000.0, stats.getMax());
        assertEquals(101_250.0, stats.getAverage());
    }

    @Test
    void averageSalaryByDepartment_computesCorrectly() {
        Map<String, Double> avg = EmployeeCollectors.averageSalaryByDepartment(sample());
        assertEquals(115_000.0, avg.get("Engineering"));
        assertEquals(87_500.0, avg.get("Marketing"));
    }
}
