using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Count occurrences of each character in a string.
// Variants: all chars, letters only, case-insensitive, sorted by frequency.

namespace Recon.CodingChallenges
{
    public class CharacterFrequency
    {
        // ─── Basic: Dictionary approach ───────────────────────────────────
        public static Dictionary<char, int> Count(string s)
        {
            var freq = new Dictionary<char, int>();
            foreach (var c in s)
                freq[c] = freq.TryGetValue(c, out var count) ? count + 1 : 1;
            return freq;
        }

        // ─── LINQ approach ────────────────────────────────────────────────
        public static Dictionary<char, int> CountLinq(string s)
            => s.GroupBy(c => c).ToDictionary(g => g.Key, g => g.Count());

        // ─── Letters only, case-insensitive ──────────────────────────────
        public static Dictionary<char, int> CountLetters(string s)
            => s.ToLowerInvariant()
               .Where(char.IsLetter)
               .GroupBy(c => c)
               .ToDictionary(g => g.Key, g => g.Count());

        // ─── Top N most frequent characters ──────────────────────────────
        public static IEnumerable<(char Char, int Count)> TopN(string s, int n)
            => CountLinq(s)
               .OrderByDescending(kv => kv.Value)
               .Take(n)
               .Select(kv => (kv.Key, kv.Value));

        // ─── As sorted string (useful for anagram check) ─────────────────
        public static string SortedChars(string s)
            => new string(s.ToLowerInvariant().Where(char.IsLetter).OrderBy(c => c).ToArray());
    }

    class Program
    {
        static void Main()
        {
            var s = "Hello World";

            Console.WriteLine("─── Character frequency ──────────────────────────");
            foreach (var (ch, count) in CharacterFrequency.CountLinq(s).OrderBy(kv => kv.Key))
                Console.WriteLine($"  '{(ch == ' ' ? "space" : ch.ToString())}': {count}");

            Console.WriteLine("\n─── Letters only (case-insensitive) ─────────────");
            foreach (var (ch, count) in CharacterFrequency.CountLetters(s).OrderBy(kv => kv.Key))
                Console.WriteLine($"  '{ch}': {count}");

            Console.WriteLine("\n─── Top 3 most frequent chars ───────────────────");
            foreach (var (ch, count) in CharacterFrequency.TopN(s, 3))
                Console.WriteLine($"  '{(ch == ' ' ? "space" : ch.ToString())}': {count}");

            Console.WriteLine("\n─── Sorted chars (anagram key) ──────────────────");
            Console.WriteLine($"  'Hello World': '{CharacterFrequency.SortedChars(s)}'");
            Console.WriteLine($"  'listen':      '{CharacterFrequency.SortedChars("listen")}'");
            Console.WriteLine($"  'silent':      '{CharacterFrequency.SortedChars("silent")}'");
        }
    }
}
