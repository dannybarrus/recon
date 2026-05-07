using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Reverse a string.
// Variants: simple reverse, word-level reverse, sentence reverse.

namespace Recon.CodingChallenges
{
    public class ReverseString
    {
        // ─── Basic reverse ────────────────────────────────────────────────
        public static string Reverse(string s)
        {
            if (string.IsNullOrEmpty(s)) return s;
            var chars = s.ToCharArray();
            Array.Reverse(chars);
            return new string(chars);
        }

        // ─── LINQ approach ────────────────────────────────────────────────
        public static string ReverseLinq(string s)
            => string.IsNullOrEmpty(s) ? s : new string(s.Reverse().ToArray());

        // ─── In-place two-pointer (interview favourite) ───────────────────
        public static string ReverseTwoPointer(string s)
        {
            if (string.IsNullOrEmpty(s)) return s;
            var chars = s.ToCharArray();
            int left = 0, right = chars.Length - 1;
            while (left < right)
            {
                (chars[left], chars[right]) = (chars[right], chars[left]);
                left++; right--;
            }
            return new string(chars);
        }

        // ─── Reverse each word, keep word order ───────────────────────────
        public static string ReverseEachWord(string s)
            => string.Join(' ', s.Split(' ').Select(w => Reverse(w)));

        // ─── Reverse word order ───────────────────────────────────────────
        public static string ReverseWordOrder(string s)
            => string.Join(' ', s.Split(' ', StringSplitOptions.RemoveEmptyEntries).Reverse());
    }

    class Program
    {
        static void Main()
        {
            var s = "Hello World";

            Console.WriteLine("─── Reverse string ──────────────────────────────");
            Console.WriteLine($"  Array.Reverse:  {ReverseString.Reverse(s)}");
            Console.WriteLine($"  LINQ:           {ReverseString.ReverseLinq(s)}");
            Console.WriteLine($"  Two pointer:    {ReverseString.ReverseTwoPointer(s)}");

            Console.WriteLine("\n─── Reverse each word ───────────────────────────");
            Console.WriteLine($"  '{s}' -> '{ReverseString.ReverseEachWord(s)}'");

            Console.WriteLine("\n─── Reverse word order ──────────────────────────");
            Console.WriteLine($"  '{s}' -> '{ReverseString.ReverseWordOrder(s)}'");

            var sentence = "the quick brown fox";
            Console.WriteLine($"  '{sentence}' -> '{ReverseString.ReverseWordOrder(sentence)}'");
        }
    }
}
