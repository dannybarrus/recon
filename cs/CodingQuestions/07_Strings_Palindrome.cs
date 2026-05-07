using System;
using System.Linq;
using System.Text.RegularExpressions;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Check if a string is a palindrome.
// Variants: basic, case-insensitive, ignoring spaces and punctuation, numeric.

namespace Recon.CodingChallenges
{
    public class Palindrome
    {
        // ─── Basic: exact character match ─────────────────────────────────
        public static bool IsBasic(string s)
        {
            if (string.IsNullOrEmpty(s)) return true;
            var chars = s.ToCharArray();
            Array.Reverse(chars);
            return s == new string(chars);
        }

        // ─── Two pointer: O(n/2), no extra array ──────────────────────────
        public static bool IsTwoPointer(string s)
        {
            if (string.IsNullOrEmpty(s)) return true;
            int left = 0, right = s.Length - 1;
            while (left < right)
            {
                if (s[left] != s[right]) return false;
                left++; right--;
            }
            return true;
        }

        // ─── Case-insensitive ─────────────────────────────────────────────
        public static bool IsCaseInsensitive(string s)
        {
            if (string.IsNullOrEmpty(s)) return true;
            s = s.ToLowerInvariant();
            return IsTwoPointer(s);
        }

        // ─── Ignore spaces and punctuation (real-world sentences) ─────────
        public static bool IsNormalized(string s)
        {
            if (string.IsNullOrEmpty(s)) return true;
            var normalized = new string(s.ToLowerInvariant()
                .Where(char.IsLetterOrDigit).ToArray());
            return IsTwoPointer(normalized);
        }

        // ─── Is numeric palindrome ────────────────────────────────────────
        public static bool IsNumericPalindrome(int n)
        {
            if (n < 0) return false;
            var s = n.ToString();
            return IsTwoPointer(s);
        }
    }

    class Program
    {
        static void Main()
        {
            Console.WriteLine("─── Basic palindrome ────────────────────────────");
            Console.WriteLine($"  'racecar':  {Palindrome.IsBasic("racecar")}");
            Console.WriteLine($"  'hello':    {Palindrome.IsBasic("hello")}");
            Console.WriteLine($"  'a':        {Palindrome.IsBasic("a")}");
            Console.WriteLine($"  '':         {Palindrome.IsBasic("")}");

            Console.WriteLine("\n─── Case insensitive ─────────────────────────────");
            Console.WriteLine($"  'Racecar':  {Palindrome.IsCaseInsensitive("Racecar")}");
            Console.WriteLine($"  'Hello':    {Palindrome.IsCaseInsensitive("Hello")}");

            Console.WriteLine("\n─── Normalized (ignore spaces/punctuation) ───────");
            Console.WriteLine($"  'A man a plan a canal Panama': {Palindrome.IsNormalized("A man a plan a canal Panama")}");
            Console.WriteLine($"  'Was it a car or a cat I saw': {Palindrome.IsNormalized("Was it a car or a cat I saw")}");
            Console.WriteLine($"  'Hello World':                 {Palindrome.IsNormalized("Hello World")}");

            Console.WriteLine("\n─── Numeric palindrome ───────────────────────────");
            Console.WriteLine($"  121:   {Palindrome.IsNumericPalindrome(121)}");
            Console.WriteLine($"  12321: {Palindrome.IsNumericPalindrome(12321)}");
            Console.WriteLine($"  123:   {Palindrome.IsNumericPalindrome(123)}");
            Console.WriteLine($"  -121:  {Palindrome.IsNumericPalindrome(-121)}");
        }
    }
}
