using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE 16 ─────────────────────────────────────────────────────────────
// FizzBuzz — clean implementations showing modern C# idioms.

namespace Recon.CodingChallenges
{
    public class FizzBuzz
    {
        // ─── Classic imperative ───────────────────────────────────────────
        public static IEnumerable<string> Generate(int n)
        {
            for (int i = 1; i <= n; i++)
            {
                if (i % 15 == 0)     yield return "FizzBuzz";
                else if (i % 3 == 0) yield return "Fizz";
                else if (i % 5 == 0) yield return "Buzz";
                else                 yield return i.ToString();
            }
        }

        // ─── Extensible: supports arbitrary rules ─────────────────────────
        public static IEnumerable<string> GenerateWithRules(
            int n, IEnumerable<(int Divisor, string Label)> rules)
        {
            for (int i = 1; i <= n; i++)
            {
                var result = string.Concat(rules
                    .Where(r => i % r.Divisor == 0)
                    .Select(r => r.Label));

                yield return string.IsNullOrEmpty(result) ? i.ToString() : result;
            }
        }

        // ─── Switch expression approach (C# 8) ───────────────────────────
        public static string ForNumber(int n) => (n % 3 == 0, n % 5 == 0) switch
        {
            (true,  true)  => "FizzBuzz",
            (true,  false) => "Fizz",
            (false, true)  => "Buzz",
            _              => n.ToString()
        };
    }

    class Program
    {
        static void Main()
        {
            Console.WriteLine("─── FizzBuzz 1–20 ───────────────────────────────");
            Console.WriteLine(string.Join(", ", FizzBuzz.Generate(20)));

            Console.WriteLine("\n─── Extensible FizzBuzz (add 7 = Bazz) ──────────");
            var rules = new[] { (3, "Fizz"), (5, "Buzz"), (7, "Bazz") };
            Console.WriteLine(string.Join(", ", FizzBuzz.GenerateWithRules(21, rules)));

            Console.WriteLine("\n─── Switch expression for single number ──────────");
            foreach (var n in new[] { 1, 3, 5, 15 })
                Console.WriteLine($"  {n,2}: {FizzBuzz.ForNumber(n)}");
        }
    }
}
