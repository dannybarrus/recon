using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Check if a number is prime.
// Variants: basic, optimized, sieve of Eratosthenes for range.

namespace Recon.CodingChallenges
{
    public class PrimeNumbers
    {
        // ─── Basic: O(sqrt n) ─────────────────────────────────────────────
        public static bool IsPrime(int n)
        {
            if (n < 2)  return false;
            if (n == 2) return true;
            if (n % 2 == 0) return false;   // eliminate even numbers early

            for (int i = 3; i * i <= n; i += 2)   // only check odd divisors to sqrt(n)
                if (n % i == 0) return false;

            return true;
        }

        // ─── Next prime after n ───────────────────────────────────────────
        public static int NextPrime(int n)
        {
            int candidate = n + 1;
            while (!IsPrime(candidate)) candidate++;
            return candidate;
        }

        // ─── All primes up to n using Sieve of Eratosthenes: O(n log log n)
        public static IEnumerable<int> Sieve(int limit)
        {
            if (limit < 2) return Enumerable.Empty<int>();

            var isComposite = new bool[limit + 1];

            for (int i = 2; i * i <= limit; i++)
                if (!isComposite[i])
                    for (int j = i * i; j <= limit; j += i)
                        isComposite[j] = true;

            return Enumerable.Range(2, limit - 1).Where(n => !isComposite[n]);
        }

        // ─── Prime factors of n ───────────────────────────────────────────
        public static IEnumerable<int> PrimeFactors(int n)
        {
            var factors = new List<int>();
            for (int d = 2; d * d <= n; d++)
                while (n % d == 0) { factors.Add(d); n /= d; }
            if (n > 1) factors.Add(n);
            return factors;
        }
    }

    class Program
    {
        static void Main()
        {
            Console.WriteLine("─── Is prime? ───────────────────────────────────");
            foreach (var n in new[] { 1, 2, 3, 4, 17, 18, 97, 100 })
                Console.WriteLine($"  {n,3}: {PrimeNumbers.IsPrime(n)}");

            Console.WriteLine("\n─── Next prime after n ──────────────────────────");
            foreach (var n in new[] { 10, 20, 50, 100 })
                Console.WriteLine($"  After {n,3}: {PrimeNumbers.NextPrime(n)}");

            Console.WriteLine("\n─── Primes up to 50 (Sieve) ─────────────────────");
            Console.WriteLine($"  {string.Join(", ", PrimeNumbers.Sieve(50))}");

            Console.WriteLine("\n─── Prime factors ───────────────────────────────");
            foreach (var n in new[] { 12, 28, 100, 97 })
                Console.WriteLine($"  {n,3}: [{string.Join(", ", PrimeNumbers.PrimeFactors(n))}]");
        }
    }
}
