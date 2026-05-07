using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Find the first non-repeating character in a string.
// Variants: char, index, all unique chars, last non-repeating.

namespace Recon.CodingChallenges
{
    public class FirstNonRepeating
    {
        // ─── Two-pass: build frequency map, then find first with count 1 ──
        public static char? Find(string s)
        {
            if (string.IsNullOrEmpty(s)) return null;

            var freq = new Dictionary<char, int>();
            foreach (var c in s)
                freq[c] = freq.TryGetValue(c, out var count) ? count + 1 : 1;

            foreach (var c in s)
                if (freq[c] == 1) return c;

            return null;
        }

        // ─── Return index instead of char ─────────────────────────────────
        public static int FindIndex(string s)
        {
            if (string.IsNullOrEmpty(s)) return -1;

            var freq = s.GroupBy(c => c).ToDictionary(g => g.Key, g => g.Count());

            for (int i = 0; i < s.Length; i++)
                if (freq[s[i]] == 1) return i;

            return -1;
        }

        // ─── LINQ one-liner ───────────────────────────────────────────────
        public static char? FindLinq(string s)
            => string.IsNullOrEmpty(s)
                ? null
                : s.GroupBy(c => c)
                   .FirstOrDefault(g => g.Count() == 1)?.Key;

        // ─── All unique characters in order of first appearance ───────────
        public static IEnumerable<char> AllUnique(string s)
        {
            var freq = s.GroupBy(c => c).ToDictionary(g => g.Key, g => g.Count());
            return s.Where(c => freq[c] == 1).Distinct();
        }

        // ─── Last non-repeating character ─────────────────────────────────
        public static char? FindLast(string s)
        {
            if (string.IsNullOrEmpty(s)) return null;
            var freq = s.GroupBy(c => c).ToDictionary(g => g.Key, g => g.Count());
            return s.LastOrDefault(c => freq[c] == 1);
        }
    }

    class Program
    {
        static void Main()
        {
            var tests = new[] { "aabbcde", "aabb", "leetcode", "loveleetcode", "z" };

            Console.WriteLine("─── First non-repeating character ───────────────");
            foreach (var s in tests)
            {
                var result = FirstNonRepeating.Find(s);
                Console.WriteLine($"  '{s,-15}' -> {(result.HasValue ? $"'{result}'" : "none")}");
            }

            Console.WriteLine("\n─── First non-repeating index ───────────────────");
            foreach (var s in tests)
                Console.WriteLine($"  '{s,-15}' -> index {FirstNonRepeating.FindIndex(s)}");

            Console.WriteLine("\n─── All unique chars in order ───────────────────");
            var s2 = "loveleetcode";
            Console.WriteLine($"  '{s2}' -> [{string.Join(", ", FirstNonRepeating.AllUnique(s2).Select(c => $"'{c}'"))}]");

            Console.WriteLine("\n─── Last non-repeating char ─────────────────────");
            foreach (var s in tests)
            {
                var result = FirstNonRepeating.FindLast(s);
                Console.WriteLine($"  '{s,-15}' -> {(result.HasValue ? $"'{result}'" : "none")}");
            }
        }
    }
}
