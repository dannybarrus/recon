using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Check if two strings are anagrams of each other.
// Variants: sort approach, frequency map, group anagrams from a list.

namespace Recon.CodingChallenges
{
    public class Anagram
    {
        // ─── Sort approach: O(n log n) ────────────────────────────────────
        public static bool IsAnagramSort(string a, string b)
        {
            if (a.Length != b.Length) return false;
            return SortedKey(a) == SortedKey(b);
        }

        // ─── Frequency map: O(n) ──────────────────────────────────────────
        public static bool IsAnagramFrequency(string a, string b)
        {
            if (a.Length != b.Length) return false;

            var freq = new Dictionary<char, int>();

            foreach (var c in a)
                freq[c] = freq.TryGetValue(c, out var v) ? v + 1 : 1;

            foreach (var c in b)
            {
                if (!freq.TryGetValue(c, out var v) || v == 0) return false;
                freq[c] = v - 1;
            }

            return true;
        }

        // ─── Case-insensitive, ignore spaces ──────────────────────────────
        public static bool IsAnagramNormalized(string a, string b)
        {
            var normalize = (string s) => new string(
                s.ToLowerInvariant().Where(char.IsLetter).OrderBy(c => c).ToArray());
            return normalize(a) == normalize(b);
        }

        // ─── Group a list of strings by anagram family ────────────────────
        public static IEnumerable<IGrouping<string, string>> GroupAnagrams(IEnumerable<string> words)
            => words.GroupBy(SortedKey);

        private static string SortedKey(string s)
            => new string(s.ToLowerInvariant().OrderBy(c => c).ToArray());
    }

    class Program
    {
        static void Main()
        {
            Console.WriteLine("─── Is anagram (sort) ───────────────────────────");
            Console.WriteLine($"  listen / silent:  {Anagram.IsAnagramSort("listen", "silent")}");
            Console.WriteLine($"  triangle / integral: {Anagram.IsAnagramSort("triangle", "integral")}");
            Console.WriteLine($"  hello / world:    {Anagram.IsAnagramSort("hello", "world")}");
            Console.WriteLine($"  abc / ab:         {Anagram.IsAnagramSort("abc", "ab")}");

            Console.WriteLine("\n─── Is anagram (frequency map) ──────────────────");
            Console.WriteLine($"  listen / silent:  {Anagram.IsAnagramFrequency("listen", "silent")}");
            Console.WriteLine($"  rat / car:        {Anagram.IsAnagramFrequency("rat", "car")}");

            Console.WriteLine("\n─── Normalized (case/spaces ignored) ────────────");
            Console.WriteLine($"  'Astronomer' / 'Moon starer': {Anagram.IsAnagramNormalized("Astronomer", "Moon starer")}");

            Console.WriteLine("\n─── Group anagrams ──────────────────────────────");
            var words = new[] { "eat", "tea", "tan", "ate", "nat", "bat" };
            foreach (var group in Anagram.GroupAnagrams(words))
                Console.WriteLine($"  [{string.Join(", ", group)}]");
        }
    }
}
