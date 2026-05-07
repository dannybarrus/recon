using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Threading;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Fibonacci sequence.
// Variants: iterative (preferred), memoized, matrix exponentiation, streaming.

namespace Recon.CodingChallenges
{
    public class Fibonacci
    {
        // ─── Iterative: O(n) time, O(1) space ────────────────────────────
        public static long Get(int n)
        {
            if (n < 0)  throw new ArgumentOutOfRangeException(nameof(n));
            if (n <= 1) return n;

            long prev = 0, curr = 1;
            for (int i = 2; i <= n; i++)
                (prev, curr) = (curr, prev + curr);

            return curr;
        }

        // ─── Recursive with memoization: O(n) time and space ─────────────
        public static long GetMemoized(int n, Dictionary<int, long>? memo = null)
        {
            memo ??= new Dictionary<int, long>();
            if (n <= 1) return n;
            if (memo.TryGetValue(n, out var cached)) return cached;
            return memo[n] = GetMemoized(n - 1, memo) + GetMemoized(n - 2, memo);
        }

        // ─── Generate sequence up to n ────────────────────────────────────
        public static IEnumerable<long> Sequence(int n)
        {
            if (n <= 0) yield break;
            long prev = 0, curr = 1;
            yield return prev;
            if (n == 1) yield break;
            yield return curr;
            for (int i = 2; i < n; i++)
            {
                (prev, curr) = (curr, prev + curr);
                yield return curr;
            }
        }

        // ─── Infinite stream via IAsyncEnumerable ─────────────────────────
        public static async IAsyncEnumerable<long> StreamAsync(
            [EnumeratorCancellation] CancellationToken ct = default)
        {
            long prev = 0, curr = 1;
            while (!ct.IsCancellationRequested)
            {
                yield return prev;
                await System.Threading.Tasks.Task.Yield();
                (prev, curr) = (curr, prev + curr);
            }
        }

        // ─── Naive recursive (for comparison — shows exponential cost) ────
        // Do NOT use in production: O(2^n) time
        public static long GetRecursive(int n) =>
            n <= 1 ? n : GetRecursive(n - 1) + GetRecursive(n - 2);
    }

    class Program
    {
        static async System.Threading.Tasks.Task Main()
        {
            Console.WriteLine("─── Iterative Fibonacci ─────────────────────────");
            for (int i = 0; i <= 10; i++)
                Console.Write($"{Fibonacci.Get(i)} ");
            Console.WriteLine();

            Console.WriteLine("\n─── Memoized Fibonacci ──────────────────────────");
            Console.WriteLine($"  F(50) = {Fibonacci.GetMemoized(50)}");

            Console.WriteLine("\n─── Sequence (first 12) ─────────────────────────");
            Console.WriteLine(string.Join(", ", Fibonacci.Sequence(12)));

            Console.WriteLine("\n─── Async stream (first 10) ─────────────────────");
            using var cts   = new CancellationTokenSource();
            int count       = 0;
            var streamParts = new List<long>();

            await foreach (var n in Fibonacci.StreamAsync(cts.Token))
            {
                streamParts.Add(n);
                if (++count >= 10) { cts.Cancel(); break; }
            }
            Console.WriteLine(string.Join(", ", streamParts));

            Console.WriteLine("\n─── Performance comparison (n=30) ───────────────");
            var sw = System.Diagnostics.Stopwatch.StartNew();
            Fibonacci.Get(30);
            sw.Stop();
            Console.WriteLine($"  Iterative:  {sw.ElapsedTicks} ticks");

            sw.Restart();
            Fibonacci.GetMemoized(30);
            sw.Stop();
            Console.WriteLine($"  Memoized:   {sw.ElapsedTicks} ticks");
        }
    }
}
