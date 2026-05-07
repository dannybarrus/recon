using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Flatten nested collections using SelectMany.
// Variants: simple flatten, flatten with projection, flatten with filter.

namespace Recon.CodingChallenges
{
    public class Order
    {
        public int         Id        { get; set; }
        public string      Customer  { get; set; } = string.Empty;
        public List<Item>  LineItems { get; set; } = new();
    }

    public class Item
    {
        public string  Name     { get; set; } = string.Empty;
        public decimal Price    { get; set; }
        public int     Quantity { get; set; }
    }

    public class FlattenNestedList
    {
        // ─── Basic flatten: one order -> many items, all flattened ────────
        public static IEnumerable<Item> FlattenLineItems(IEnumerable<Order> orders)
            => orders.SelectMany(o => o.LineItems);

        // ─── Flatten with projection: include parent context ──────────────
        public static IEnumerable<(string Customer, string Item, decimal Price)>
            FlattenWithContext(IEnumerable<Order> orders)
            => orders.SelectMany(
                o => o.LineItems,
                (order, item) => (order.Customer, item.Name, item.Price));

        // ─── Flatten with filter ──────────────────────────────────────────
        public static IEnumerable<Item> ExpensiveItems(IEnumerable<Order> orders, decimal threshold)
            => orders.SelectMany(o => o.LineItems).Where(i => i.Price > threshold);

        // ─── Flatten nested strings (classic) ────────────────────────────
        public static IEnumerable<char> FlattenStrings(IEnumerable<string> words)
            => words.SelectMany(w => w);

        // ─── Flatten jagged array ─────────────────────────────────────────
        public static IEnumerable<T> Flatten<T>(IEnumerable<IEnumerable<T>> nested)
            => nested.SelectMany(x => x);
    }

    class Program
    {
        static void Main()
        {
            var orders = new List<Order>
            {
                new() { Id = 1, Customer = "Alice", LineItems = new List<Item>
                    {
                        new() { Name = "Widget",  Price = 9.99m,  Quantity = 2 },
                        new() { Name = "Gadget",  Price = 49.99m, Quantity = 1 },
                    }
                },
                new() { Id = 2, Customer = "Bob", LineItems = new List<Item>
                    {
                        new() { Name = "Widget",  Price = 9.99m,  Quantity = 5 },
                        new() { Name = "Doohickey", Price = 24.99m, Quantity = 1 },
                        new() { Name = "Thingamajig", Price = 99.99m, Quantity = 1 },
                    }
                },
            };

            Console.WriteLine("─── All line items (flattened) ──────────────────");
            foreach (var item in FlattenNestedList.FlattenLineItems(orders))
                Console.WriteLine($"  {item.Name,-15} ${item.Price:N2}");

            Console.WriteLine("\n─── With customer context ────────────────────────");
            foreach (var (customer, name, price) in FlattenNestedList.FlattenWithContext(orders))
                Console.WriteLine($"  {customer,-8} {name,-15} ${price:N2}");

            Console.WriteLine("\n─── Expensive items (> $25) ─────────────────────");
            foreach (var item in FlattenNestedList.ExpensiveItems(orders, 25m))
                Console.WriteLine($"  {item.Name,-15} ${item.Price:N2}");

            Console.WriteLine("\n─── Flatten strings to chars ────────────────────");
            var words  = new[] { "Hello", "World" };
            var chars  = FlattenNestedList.FlattenStrings(words);
            Console.WriteLine($"  {string.Join(", ", chars)}");

            Console.WriteLine("\n─── Flatten jagged int array ────────────────────");
            var jagged = new[] { new[] { 1, 2 }, new[] { 3, 4, 5 }, new[] { 6 } };
            Console.WriteLine($"  {string.Join(", ", FlattenNestedList.Flatten(jagged))}");
        }
    }
}
