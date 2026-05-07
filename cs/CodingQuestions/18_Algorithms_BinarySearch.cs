using System;
using System.Collections.Generic;

// ─── CHALLENGE 18 ─────────────────────────────────────────────────────────────
// Binary search on a sorted list.

namespace Recon.CodingChallenges
{
    public class BinarySearch
    {
        // ─── Iterative: O(log n), preferred over recursive ────────────────
        public static int Search<T>(T[] sortedArray, T target) where T : IComparable<T>
        {
            int left = 0, right = sortedArray.Length - 1;

            while (left <= right)
            {
                int mid = left + (right - left) / 2;    // avoids overflow vs (left+right)/2
                int cmp = sortedArray[mid].CompareTo(target);

                if      (cmp == 0) return mid;           // found
                else if (cmp < 0)  left  = mid + 1;     // target is right of mid
                else               right = mid - 1;     // target is left of mid
            }

            return -1;  // not found
        }

        // ─── Recursive ────────────────────────────────────────────────────
        public static int SearchRecursive<T>(T[] arr, T target, int left, int right)
            where T : IComparable<T>
        {
            if (left > right) return -1;

            int mid = left + (right - left) / 2;
            int cmp = arr[mid].CompareTo(target);

            return cmp == 0 ? mid
                 : cmp < 0  ? SearchRecursive(arr, target, mid + 1, right)
                             : SearchRecursive(arr, target, left, mid - 1);
        }

        // ─── Find first occurrence of duplicate target ────────────────────
        public static int SearchFirst<T>(T[] arr, T target) where T : IComparable<T>
        {
            int left = 0, right = arr.Length - 1, result = -1;

            while (left <= right)
            {
                int mid = left + (right - left) / 2;
                int cmp = arr[mid].CompareTo(target);

                if      (cmp == 0) { result = mid; right = mid - 1; }  // keep searching left
                else if (cmp < 0)  left  = mid + 1;
                else               right = mid - 1;
            }
            return result;
        }
    }

    class Program
    {
        static void Main()
        {
            var arr = new[] { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };

            Console.WriteLine("─── Binary search (iterative) ───────────────────");
            Console.WriteLine($"  Search 7:   index {BinarySearch.Search(arr, 7)}");
            Console.WriteLine($"  Search 1:   index {BinarySearch.Search(arr, 1)}");
            Console.WriteLine($"  Search 19:  index {BinarySearch.Search(arr, 19)}");
            Console.WriteLine($"  Search 6:   index {BinarySearch.Search(arr, 6)}  (not found)");

            Console.WriteLine("\n─── Recursive ───────────────────────────────────");
            Console.WriteLine($"  Search 11:  index {BinarySearch.SearchRecursive(arr, 11, 0, arr.Length - 1)}");

            Console.WriteLine("\n─── Find first occurrence ───────────────────────");
            var dups = new[] { 1, 2, 2, 2, 3, 4, 5 };
            Console.WriteLine($"  First '2' in [1,2,2,2,3,4,5]: index {BinarySearch.SearchFirst(dups, 2)}");

            Console.WriteLine("\n─── Strings (any IComparable<T>) ────────────────");
            var words = new[] { "apple", "banana", "cherry", "date", "elderberry" };
            Console.WriteLine($"  Search 'cherry': index {BinarySearch.Search(words, "cherry")}");
            Console.WriteLine($"  Search 'fig':    index {BinarySearch.Search(words, "fig")}");
        }
    }
}
