using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Given a list of employees, return the top earner per department.
// Extension: return the top N earners per department.

namespace Recon.CodingChallenges
{
    public class Employee
    {
        public string Name       { get; set; } = string.Empty;
        public string Department { get; set; } = string.Empty;
        public decimal Salary    { get; set; }
    }

    public class TopEarnerByDepartment
    {
        // ─── Solution 1: top earner per department ────────────────────────
        public static IEnumerable<Employee> GetTopEarners(IEnumerable<Employee> employees)
        {
            if (employees is null) throw new ArgumentNullException(nameof(employees));

            return employees
                .GroupBy(e => e.Department)
                .Select(g => g.OrderByDescending(e => e.Salary).First());
        }

        // ─── Solution 2: top N earners per department ─────────────────────
        public static IEnumerable<Employee> GetTopNEarners(IEnumerable<Employee> employees, int n)
        {
            if (employees is null) throw new ArgumentNullException(nameof(employees));
            if (n <= 0) throw new ArgumentOutOfRangeException(nameof(n));

            return employees
                .GroupBy(e => e.Department)
                .SelectMany(g => g.OrderByDescending(e => e.Salary).Take(n));
        }

        // ─── Solution 3: department summary ──────────────────────────────
        public static IEnumerable<(string Department, decimal Average, decimal Max, int Count)>
            GetDepartmentSummary(IEnumerable<Employee> employees)
        {
            return employees
                .GroupBy(e => e.Department)
                .Select(g => (
                    Department: g.Key,
                    Average:    Math.Round(g.Average(e => e.Salary), 2),
                    Max:        g.Max(e => e.Salary),
                    Count:      g.Count()
                ))
                .OrderBy(d => d.Department);
        }
    }

    // ─── DOTNETFIDDLE ENTRY POINT ─────────────────────────────────────────────
    class Program
    {
        static void Main()
        {
            var employees = new List<Employee>
            {
                new() { Name = "Alice",   Department = "Engineering", Salary = 120_000 },
                new() { Name = "Bob",     Department = "Engineering", Salary = 95_000  },
                new() { Name = "Carol",   Department = "Engineering", Salary = 110_000 },
                new() { Name = "Dave",    Department = "Marketing",   Salary = 85_000  },
                new() { Name = "Eve",     Department = "Marketing",   Salary = 90_000  },
                new() { Name = "Frank",   Department = "Finance",     Salary = 105_000 },
                new() { Name = "Grace",   Department = "Finance",     Salary = 115_000 },
            };

            Console.WriteLine("─── Top earner per department ───────────────────");
            foreach (var e in TopEarnerByDepartment.GetTopEarners(employees))
                Console.WriteLine($"  {e.Department,-15} {e.Name,-10} ${e.Salary:N0}");

            Console.WriteLine("\n─── Top 2 earners per department ────────────────");
            foreach (var e in TopEarnerByDepartment.GetTopNEarners(employees, 2))
                Console.WriteLine($"  {e.Department,-15} {e.Name,-10} ${e.Salary:N0}");

            Console.WriteLine("\n─── Department summary ───────────────────────────");
            foreach (var (dept, avg, max, count) in TopEarnerByDepartment.GetDepartmentSummary(employees))
                Console.WriteLine($"  {dept,-15} Count: {count}  Avg: ${avg:N0}  Max: ${max:N0}");
        }
    }
}
