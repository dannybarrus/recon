using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Find the missing number in a sequence 1 to N.
// Variants: sum formula (O(1) space), XOR, multiple missing, unsorted.

namespace Recon.CodingChallenges
{
    public class MissingNumber
    {
        // ─── Sum formula: O(n) time, O(1) space ──────────────────────────
        // Expected sum of 1..n = n*(n+1)/2. Subtract actual sum to find gap.
        public static int FindBySumFormula(int[] nums)
        {
            int n           = nums.Length + 1;
            long expected   = (long)n * (n + 1) / 2;
            long actual     = nums.Sum(x => (long)x);
            return (int)(expected - actual);
        }

        // ─── XOR: O(n) time, O(1) space, no overflow risk ─────────────────
        // XOR of all indices 1..n XOR all values: equal values cancel out.
        public static int FindByXor(int[] nums)
        {
            int result = nums.Length + 1;   // start with n (the full range count)
            for (int i = 0; i < nums.Length; i++)
                result ^= (i + 1) ^ nums[i];
            return result;
        }

        // ─── HashSet: intuitive, O(n) space ───────────────────────────────
        public static int FindByHashSet(int[] nums)
        {
            var set = new HashSet<int>(nums);
            for (int i = 1; i <= nums.Length + 1; i++)
                if (!set.Contains(i)) return i;
            return -1;
        }

        // ─── Multiple missing numbers ──────────────────────────────────────
        public static IEnumerable<int> FindAllMissing(int[] nums, int n)
        {
            var set = new HashSet<int>(nums);
            return Enumerable.Range(1, n).Where(i => !set.Contains(i));
        }

        // ─── Missing in unsorted array with duplicates ────────────────────
        public static IEnumerable<int> FindMissingUnsorted(int[] nums)
        {
            int n   = nums.Length;
            var set = new HashSet<int>(nums);
            return Enumerable.Range(1, n).Where(i => !set.Contains(i));
        }
    }

    class Program
    {
        static void Main()
        {
            var arr = new[] { 1, 2, 4, 5, 6 };   // missing 3

            Console.WriteLine("─── Find single missing number ───────────────────");
            Console.WriteLine($"  Sum formula:  {MissingNumber.FindBySumFormula(arr)}");
            Console.WriteLine($"  XOR:          {MissingNumber.FindByXor(arr)}");
            Console.WriteLine($"  HashSet:      {MissingNumber.FindByHashSet(arr)}");

            Console.WriteLine("\n─── Edge cases ──────────────────────────────────");
            Console.WriteLine($"  Missing 1: {MissingNumber.FindBySumFormula(new[] { 2, 3, 4, 5 })}");
            Console.WriteLine($"  Missing 5: {MissingNumber.FindBySumFormula(new[] { 1, 2, 3, 4 })}");
            Console.WriteLine($"  Single [2]: {MissingNumber.FindBySumFormula(new[] { 2 })}");

            Console.WriteLine("\n─── Multiple missing numbers ─────────────────────");
            var withGaps = new[] { 1, 2, 5, 7, 9 };
            Console.WriteLine($"  Missing from 1..9: [{string.Join(", ", MissingNumber.FindAllMissing(withGaps, 9))}]");

            Console.WriteLine("\n─── Unsorted with duplicates ─────────────────────");
            var unsorted = new[] { 4, 3, 2, 7, 8, 2, 3, 1 };  // missing 5, 6
            Console.WriteLine($"  [{string.Join(", ", MissingNumber.FindMissingUnsorted(unsorted))}]");
        }
    }
}
