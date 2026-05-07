using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Find duplicates in a collection.
// Variants: any type, custom objects, return distinct duplicates vs all occurrences.

namespace Recon.CodingChallenges
{
    public class FindDuplicates
    {
        // ─── Distinct duplicates: each duplicate value returned once ──────
        public static IEnumerable<T> GetDistinctDuplicates<T>(IEnumerable<T> items)
            => items
                .GroupBy(x => x)
                .Where(g => g.Count() > 1)
                .Select(g => g.Key);

        // ─── Using HashSet: O(n), returns first-seen duplicates ───────────
        public static IEnumerable<T> GetDuplicatesWithHashSet<T>(IEnumerable<T> items)
        {
            var seen = new HashSet<T>();
            var duplicates = new HashSet<T>();

            foreach (var item in items)
                if (!seen.Add(item))
                    duplicates.Add(item);

            return duplicates;
        }

        // ─── All duplicate occurrences (not just first) ───────────────────
        public static IEnumerable<T> GetAllDuplicateOccurrences<T>(IEnumerable<T> items)
        {
            var duplicateKeys = GetDistinctDuplicates(items).ToHashSet();
            return items.Where(x => duplicateKeys.Contains(x));
        }

        // ─── Duplicates by property (custom objects) ──────────────────────
        public static IEnumerable<TItem> GetDuplicatesBy<TItem, TKey>(
            IEnumerable<TItem> items, Func<TItem, TKey> keySelector)
            => items
                .GroupBy(keySelector)
                .Where(g => g.Count() > 1)
                .SelectMany(g => g);

        // ─── Check if any duplicates exist ────────────────────────────────
        public static bool HasDuplicates<T>(IEnumerable<T> items)
        {
            var seen = new HashSet<T>();
            return items.Any(item => !seen.Add(item));
        }
    }

    public class Employee
    {
        public string Name       { get; set; } = string.Empty;
        public string Department { get; set; } = string.Empty;
    }

    class Program
    {
        static void Main()
        {
            var numbers = new[] { 1, 2, 3, 2, 4, 3, 5, 1, 6 };

            Console.WriteLine("─── Distinct duplicates (GroupBy) ───────────────");
            Console.WriteLine($"  {string.Join(", ", FindDuplicates.GetDistinctDuplicates(numbers))}");

            Console.WriteLine("\n─── Distinct duplicates (HashSet) ───────────────");
            Console.WriteLine($"  {string.Join(", ", FindDuplicates.GetDuplicatesWithHashSet(numbers))}");

            Console.WriteLine("\n─── All duplicate occurrences ────────────────────");
            Console.WriteLine($"  {string.Join(", ", FindDuplicates.GetAllDuplicateOccurrences(numbers))}");

            Console.WriteLine("\n─── Has duplicates? ──────────────────────────────");
            Console.WriteLine($"  numbers: {FindDuplicates.HasDuplicates(numbers)}");
            Console.WriteLine($"  [1,2,3]: {FindDuplicates.HasDuplicates(new[] { 1, 2, 3 })}");

            Console.WriteLine("\n─── Duplicates by property (Department) ──────────");
            var employees = new List<Employee>
            {
                new() { Name = "Alice", Department = "Engineering" },
                new() { Name = "Bob",   Department = "Marketing"   },
                new() { Name = "Carol", Department = "Engineering" },
                new() { Name = "Dave",  Department = "Finance"     },
                new() { Name = "Eve",   Department = "Marketing"   },
            };

            foreach (var e in FindDuplicates.GetDuplicatesBy(employees, e => e.Department))
                Console.WriteLine($"  {e.Name,-8} ({e.Department})");
        }
    }
}
