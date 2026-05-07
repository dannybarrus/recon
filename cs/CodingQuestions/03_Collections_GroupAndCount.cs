using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Group items and return counts.
// Variants: word frequency, order counts by status, most common items.

namespace Recon.CodingChallenges
{
    public enum OrderStatus { Pending, Processing, Shipped, Delivered, Cancelled }

    public class Order
    {
        public int         Id       { get; set; }
        public OrderStatus Status   { get; set; }
        public string      Customer { get; set; } = string.Empty;
        public decimal     Total    { get; set; }
    }

    public class GroupAndCount
    {
        // ─── Word frequency ───────────────────────────────────────────────
        public static Dictionary<string, int> WordFrequency(string text)
        {
            if (string.IsNullOrWhiteSpace(text)) return new Dictionary<string, int>();

            return text
                .Split(' ', StringSplitOptions.RemoveEmptyEntries)
                .Select(w => w.Trim().ToLowerInvariant())
                .GroupBy(w => w)
                .ToDictionary(g => g.Key, g => g.Count());
        }

        public static IEnumerable<(string Word, int Count)> TopNWords(string text, int n)
        {
            return WordFrequency(text)
                .OrderByDescending(kv => kv.Value)
                .Take(n)
                .Select(kv => (kv.Key, kv.Value));
        }

        // ─── Order counts by status ───────────────────────────────────────
        public static Dictionary<OrderStatus, int> CountByStatus(IEnumerable<Order> orders)
        {
            return orders
                .GroupBy(o => o.Status)
                .ToDictionary(g => g.Key, g => g.Count());
        }

        // ─── Revenue by customer ──────────────────────────────────────────
        public static IEnumerable<(string Customer, decimal Revenue, int OrderCount)>
            RevenueByCustomer(IEnumerable<Order> orders)
        {
            return orders
                .GroupBy(o => o.Customer)
                .Select(g => (
                    Customer:   g.Key,
                    Revenue:    g.Sum(o => o.Total),
                    OrderCount: g.Count()
                ))
                .OrderByDescending(c => c.Revenue);
        }

        // ─── Find duplicates ──────────────────────────────────────────────
        public static IEnumerable<T> FindDuplicates<T>(IEnumerable<T> items)
        {
            return items
                .GroupBy(x => x)
                .Where(g => g.Count() > 1)
                .Select(g => g.Key);
        }
    }

    class Program
    {
        static void Main()
        {
            // Word frequency
            var text = "the quick brown fox jumps over the lazy dog the fox";
            Console.WriteLine("─── Word frequency ──────────────────────────────");
            foreach (var (word, count) in GroupAndCount.TopNWords(text, 5))
                Console.WriteLine($"  '{word}': {count}");

            // Orders
            var orders = new List<Order>
            {
                new() { Id = 1, Status = OrderStatus.Shipped,    Customer = "Alice", Total = 250 },
                new() { Id = 2, Status = OrderStatus.Pending,    Customer = "Bob",   Total = 100 },
                new() { Id = 3, Status = OrderStatus.Delivered,  Customer = "Alice", Total = 400 },
                new() { Id = 4, Status = OrderStatus.Shipped,    Customer = "Carol", Total = 175 },
                new() { Id = 5, Status = OrderStatus.Pending,    Customer = "Bob",   Total = 300 },
                new() { Id = 6, Status = OrderStatus.Cancelled,  Customer = "Alice", Total = 50  },
                new() { Id = 7, Status = OrderStatus.Delivered,  Customer = "Carol", Total = 225 },
            };

            Console.WriteLine("\n─── Order counts by status ──────────────────────");
            foreach (var (status, count) in GroupAndCount.CountByStatus(orders))
                Console.WriteLine($"  {status,-15} {count}");

            Console.WriteLine("\n─── Revenue by customer ─────────────────────────");
            foreach (var (customer, revenue, orderCount) in GroupAndCount.RevenueByCustomer(orders))
                Console.WriteLine($"  {customer,-10} Orders: {orderCount}  Revenue: ${revenue:N0}");

            Console.WriteLine("\n─── Duplicates in list ──────────────────────────");
            var numbers = new List<int> { 1, 2, 3, 2, 4, 3, 5 };
            Console.WriteLine($"  Duplicates: {string.Join(", ", GroupAndCount.FindDuplicates(numbers))}");
        }
    }
}
