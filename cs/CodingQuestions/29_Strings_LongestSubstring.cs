using System;
using System.Collections.Generic;
using System.Linq;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Find the longest substring without repeating characters.
// Variant: return the length, return the substring, handle edge cases.
// Pattern: sliding window with a Dictionary to track last seen index.

namespace Recon.CodingChallenges
{
    public class LongestSubstring
    {
        // ─── Return the length ────────────────────────────────────────────
        // O(n) time, O(min(n, alphabet)) space
        public static int MaxLength(string s)
        {
            if (string.IsNullOrEmpty(s)) return 0;

            var seen   = new Dictionary<char, int>();
            int maxLen = 0;
            int left   = 0;

            for (int right = 0; right < s.Length; right++)
            {
                char c = s[right];

                // if char seen and still inside current window, shrink from left
                if (seen.TryGetValue(c, out int prevIndex) && prevIndex >= left)
                    left = prevIndex + 1;

                seen[c] = right;
                maxLen  = Math.Max(maxLen, right - left + 1);
            }

            return maxLen;
        }

        // ─── Return the actual substring ──────────────────────────────────
        public static string MaxSubstring(string s)
        {
            if (string.IsNullOrEmpty(s)) return s;

            var seen     = new Dictionary<char, int>();
            int maxLen   = 0;
            int maxStart = 0;
            int left     = 0;

            for (int right = 0; right < s.Length; right++)
            {
                char c = s[right];

                if (seen.TryGetValue(c, out int prevIndex) && prevIndex >= left)
                    left = prevIndex + 1;

                seen[c] = right;

                if (right - left + 1 > maxLen)
                {
                    maxLen   = right - left + 1;
                    maxStart = left;
                }
            }

            return s.Substring(maxStart, maxLen);
        }

        // ─── Return all substrings of max length (handles ties) ───────────
        public static IEnumerable<string> AllMaxSubstrings(string s)
        {
            if (string.IsNullOrEmpty(s)) return Enumerable.Empty<string>();

            int max     = MaxLength(s);
            var results = new List<string>();
            var seen    = new Dictionary<char, int>();
            int left    = 0;

            for (int right = 0; right < s.Length; right++)
            {
                char c = s[right];

                if (seen.TryGetValue(c, out int prevIndex) && prevIndex >= left)
                    left = prevIndex + 1;

                seen[c] = right;

                if (right - left + 1 == max)
                    results.Add(s.Substring(left, max));
            }

            return results.Distinct();
        }
    }

    class Program
    {
        static void Main()
        {
            var tests = new[]
            {
                "abcabcbb",   // 3  — abc
                "bbbbb",      // 1  — b
                "pwwkew",     // 3  — wke
                "abcdef",     // 6  — abcdef (no repeats at all)
                "dvdf",       // 3  — vdf
                "",           // 0  — empty
                "a",          // 1  — single char
                "aab",        // 2  — ab
            };

            Console.WriteLine("─── Length ───────────────────────────────────────");
            foreach (var s in tests)
                Console.WriteLine($"  \"{s,-12}\"  length: {LongestSubstring.MaxLength(s)}");

            Console.WriteLine("\n─── Substring ────────────────────────────────────");
            foreach (var s in tests.Where(s => s.Length > 0))
                Console.WriteLine($"  \"{s,-12}\"  -> \"{LongestSubstring.MaxSubstring(s)}\"");

            Console.WriteLine("\n─── All max substrings (handles ties) ────────────");
            var tied = new[] { "abba", "tmmzuxt", "pwwkew" };
            foreach (var s in tied)
                Console.WriteLine($"  \"{s,-12}\"  -> [{string.Join(", ", LongestSubstring.AllMaxSubstrings(s).Select(r => $"\"{r}\""))}]");
        }
    }
}
