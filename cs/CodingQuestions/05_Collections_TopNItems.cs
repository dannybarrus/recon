using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Return the top N items from a collection by a given property.
// Variants: top N, bottom N, paginate, percentile.

namespace Recon.CodingChallenges
{
    public class Product
    {
        public string  Name     { get; set; } = string.Empty;
        public decimal Price    { get; set; }
        public int     Sales    { get; set; }
        public double  Rating   { get; set; }
    }

    public class TopNItems
    {
        // ─── Top N by a property ──────────────────────────────────────────
        public static IEnumerable<T> TopN<T, TKey>(
            IEnumerable<T> items, Func<T, TKey> keySelector, int n)
            => items.OrderByDescending(keySelector).Take(n);

        // ─── Bottom N by a property ───────────────────────────────────────
        public static IEnumerable<T> BottomN<T, TKey>(
            IEnumerable<T> items, Func<T, TKey> keySelector, int n)
            => items.OrderBy(keySelector).Take(n);

        // ─── Simple pagination ────────────────────────────────────────────
        public static IEnumerable<T> Page<T, TKey>(
            IEnumerable<T> items, Func<T, TKey> keySelector, int page, int pageSize)
            => items
                .OrderByDescending(keySelector)
                .Skip((page - 1) * pageSize)
                .Take(pageSize);

        // ─── Above threshold ──────────────────────────────────────────────
        public static IEnumerable<T> AbovePercentile<T>(
            IEnumerable<T> items, Func<T, double> valueSelector, double percentile)
        {
            var list      = items.ToList();
            var threshold = list.Select(valueSelector).OrderBy(v => v)
                               .ElementAt((int)(list.Count * percentile / 100));
            return list.Where(x => valueSelector(x) >= threshold);
        }
    }

    class Program
    {
        static void Main()
        {
            var products = new List<Product>
            {
                new() { Name = "Widget A",  Price = 9.99m,  Sales = 1500, Rating = 4.2 },
                new() { Name = "Widget B",  Price = 14.99m, Sales = 800,  Rating = 4.7 },
                new() { Name = "Gadget X",  Price = 49.99m, Sales = 350,  Rating = 3.9 },
                new() { Name = "Gadget Y",  Price = 29.99m, Sales = 950,  Rating = 4.5 },
                new() { Name = "Doohickey", Price = 5.99m,  Sales = 2200, Rating = 3.5 },
                new() { Name = "Thingamajig", Price = 79.99m, Sales = 175, Rating = 4.9 },
                new() { Name = "Whatsit",   Price = 19.99m, Sales = 620,  Rating = 4.1 },
            };

            Console.WriteLine("─── Top 3 by sales ──────────────────────────────");
            foreach (var p in TopNItems.TopN(products, p => p.Sales, 3))
                Console.WriteLine($"  {p.Name,-15} Sales: {p.Sales}");

            Console.WriteLine("\n─── Top 3 by rating ─────────────────────────────");
            foreach (var p in TopNItems.TopN(products, p => p.Rating, 3))
                Console.WriteLine($"  {p.Name,-15} Rating: {p.Rating}");

            Console.WriteLine("\n─── Bottom 3 by price ───────────────────────────");
            foreach (var p in TopNItems.BottomN(products, p => p.Price, 3))
                Console.WriteLine($"  {p.Name,-15} ${p.Price:N2}");

            Console.WriteLine("\n─── Page 1 (3 per page) by sales ────────────────");
            foreach (var p in TopNItems.Page(products, p => p.Sales, page: 1, pageSize: 3))
                Console.WriteLine($"  {p.Name,-15} Sales: {p.Sales}");

            Console.WriteLine("\n─── Page 2 (3 per page) by sales ────────────────");
            foreach (var p in TopNItems.Page(products, p => p.Sales, page: 2, pageSize: 3))
                Console.WriteLine($"  {p.Name,-15} Sales: {p.Sales}");

            Console.WriteLine("\n─── Top 25th percentile by rating ───────────────");
            foreach (var p in TopNItems.AbovePercentile(products, p => p.Rating, 75))
                Console.WriteLine($"  {p.Name,-15} Rating: {p.Rating}");
        }
    }
}
