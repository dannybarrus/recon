using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Find the longest substring without repeating characters.
// 29a: char array index approach -- uses char as direct array index.
// A char is an integer under the hood (UTF-16). Cast to int and use as an
// index into a fixed-size array. No Dictionary, no hashing, no heap allocation
// beyond the two arrays.

namespace Recon.CodingChallenges
{
    public class LongestSubstringCharArray
    {
        // ─── Char array index approach ────────────────────────────────────
        // O(n) time, O(1) space (fixed 128-element array, not proportional to input)
        public static int MaxLength(string s)
        {
            if (string.IsNullOrEmpty(s)) return 0;

            char[] chars    = s.ToCharArray();
            int[]  lastSeen = new int[128];    // ASCII character set

            for (int i = 0; i < lastSeen.Length; i++)
                lastSeen[i] = -1;              // -1 means not yet seen

            int maxLen = 0;
            int left   = 0;

            for (int right = 0; right < chars.Length; right++)
            {
                int c = chars[right];          // char implicitly cast to int

                if (lastSeen[c] >= left)
                    left = lastSeen[c] + 1;

                lastSeen[c] = right;
                maxLen = Math.Max(maxLen, right - left + 1);
            }

            return maxLen;
        }

        // ─── Return the actual substring ──────────────────────────────────
        public static string MaxSubstring(string s)
        {
            if (string.IsNullOrEmpty(s)) return s;

            char[] chars    = s.ToCharArray();
            int[]  lastSeen = new int[128];

            for (int i = 0; i < lastSeen.Length; i++)
                lastSeen[i] = -1;

            int maxLen   = 0;
            int maxStart = 0;
            int left     = 0;

            for (int right = 0; right < chars.Length; right++)
            {
                int c = chars[right];

                if (lastSeen[c] >= left)
                    left = lastSeen[c] + 1;

                lastSeen[c] = right;

                if (right - left + 1 > maxLen)
                {
                    maxLen   = right - left + 1;
                    maxStart = left;
                }
            }

            return s.Substring(maxStart, maxLen);
        }

        // ─── Dictionary approach (for comparison) ─────────────────────────
        public static int MaxLengthDictionary(string s)
        {
            if (string.IsNullOrEmpty(s)) return 0;

            var seen   = new Dictionary<char, int>();
            int maxLen = 0;
            int left   = 0;

            for (int right = 0; right < s.Length; right++)
            {
                char c = s[right];

                if (seen.TryGetValue(c, out int prevIndex) && prevIndex >= left)
                    left = prevIndex + 1;

                seen[c] = right;
                maxLen  = Math.Max(maxLen, right - left + 1);
            }

            return maxLen;
        }

        // ─── Performance benchmark ────────────────────────────────────────
        public static (long CharArrayTicks, long DictionaryTicks) Benchmark(string s, int iterations)
        {
            // warmup -- prime JIT and CPU caches before measuring
            for (int i = 0; i < 100; i++) { MaxLength(s); MaxLengthDictionary(s); }

            var sw = Stopwatch.StartNew();
            for (int i = 0; i < iterations; i++) MaxLength(s);
            sw.Stop();
            long charArrayTicks = sw.ElapsedTicks;

            sw.Restart();
            for (int i = 0; i < iterations; i++) MaxLengthDictionary(s);
            sw.Stop();
            long dictTicks = sw.ElapsedTicks;

            return (charArrayTicks, dictTicks);
        }
    }

    class Program
    {
        static void Main()
        {
            var tests = new[]
            {
                "abcabcbb",
                "bbbbb",
                "pwwkew",
                "abcdef",
                "dvdf",
                "aab",
                "a",
                "",
            };

            Console.WriteLine("─── Max length (char array index) ───────────────");
            foreach (var s in tests)
                Console.WriteLine($"  \"{s,-12}\"  length: {LongestSubstringCharArray.MaxLength(s)}");

            Console.WriteLine("\n─── Max substring ───────────────────────────────");
            foreach (var s in tests.Where(s => s.Length > 0))
                Console.WriteLine($"  \"{s,-12}\"  -> \"{LongestSubstringCharArray.MaxSubstring(s)}\"");

            Console.WriteLine("\n─── Char array vs Dictionary (correctness check) ─");
            foreach (var s in tests)
            {
                int charArr = LongestSubstringCharArray.MaxLength(s);
                int dict    = LongestSubstringCharArray.MaxLengthDictionary(s);
                Console.WriteLine($"  \"{s,-12}\"  char[]: {charArr}  dict: {dict}  match: {charArr == dict}");
            }

            Console.WriteLine("\n─── Performance comparison (100k iterations) ─────");

            // short string
            var (shortCharArr, shortDict) = LongestSubstringCharArray.Benchmark("pwwkew", 100_000);
            Console.WriteLine($"  Short string  'pwwkew':");
            Console.WriteLine($"    char[]:     {shortCharArr,10} ticks");
            Console.WriteLine($"    Dictionary: {shortDict,10} ticks");
            Console.WriteLine($"    Ratio:      {(double)shortDict / shortCharArr:F2}x  (Dictionary is {(shortDict > shortCharArr ? "slower" : "faster")})");

            // long string with no repeats -- maximises window traversal
            var longStr = BuildLongTestString(500);
            var (longCharArr, longDict) = LongestSubstringCharArray.Benchmark(longStr, 100_000);
            Console.WriteLine($"\n  Long string   ({longStr.Length} chars, no repeats):");
            Console.WriteLine($"    char[]:     {longCharArr,10} ticks");
            Console.WriteLine($"    Dictionary: {longDict,10} ticks");
            Console.WriteLine($"    Ratio:      {(double)longDict / longCharArr:F2}x  (Dictionary is {(longDict > longCharArr ? "slower" : "faster")})");

            // high-repeat string -- frequent window shrinks stress the lookup path
            var repeatStr = string.Concat(Enumerable.Repeat("abcd", 125));
            var (repCharArr, repDict) = LongestSubstringCharArray.Benchmark(repeatStr, 100_000);
            Console.WriteLine($"\n  High-repeat   ({repeatStr.Length} chars, pattern 'abcd' x125):");
            Console.WriteLine($"    char[]:     {repCharArr,10} ticks");
            Console.WriteLine($"    Dictionary: {repDict,10} ticks");
            Console.WriteLine($"    Ratio:      {(double)repDict / repCharArr:F2}x  (Dictionary is {(repDict > repCharArr ? "slower" : "faster")})");

            Console.WriteLine("\n─── Why char as array index works ───────────────");
            Console.WriteLine($"  (int)'a' = {(int)'a'}");
            Console.WriteLine($"  (int)'z' = {(int)'z'}");
            Console.WriteLine($"  (int)'A' = {(int)'A'}");
            Console.WriteLine($"  (int)'0' = {(int)'0'}");
            Console.WriteLine("  A char is UTF-16 -- cast to int gives its code point.");
            Console.WriteLine("  For ASCII (0-127) this maps directly to a fixed-size array index.");
            Console.WriteLine("  No hashing, no collision handling, no heap allocation.");
        }

        static string BuildLongTestString(int length)
        {
            var sb = new StringBuilder(length);
            for (int i = 0; i < length; i++)
                sb.Append((char)(32 + (i % 95)));  // cycles through printable ASCII 32-126
            return sb.ToString();
        }
    }
}
