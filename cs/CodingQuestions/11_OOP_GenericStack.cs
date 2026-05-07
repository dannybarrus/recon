using System;
using System.Collections.Generic;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement a generic stack and queue from scratch.
// Shows: generics, type constraints, exception handling, IEnumerable.

namespace Recon.CodingChallenges
{
    // ─── Generic Stack ────────────────────────────────────────────────────────

    public class Stack<T>
    {
        private readonly List<T> _items = new();

        public int  Count     => _items.Count;
        public bool IsEmpty   => _items.Count == 0;

        public void Push(T item) => _items.Add(item);

        public T Pop()
        {
            if (IsEmpty) throw new InvalidOperationException("Stack is empty");
            var top = _items[^1];
            _items.RemoveAt(_items.Count - 1);
            return top;
        }

        public T Peek()
        {
            if (IsEmpty) throw new InvalidOperationException("Stack is empty");
            return _items[^1];
        }

        public bool TryPop(out T? item)
        {
            if (IsEmpty) { item = default; return false; }
            item = Pop();
            return true;
        }

        public bool TryPeek(out T? item)
        {
            if (IsEmpty) { item = default; return false; }
            item = Peek();
            return true;
        }

        public void Clear() => _items.Clear();

        public IEnumerable<T> ToEnumerable()
        {
            for (int i = _items.Count - 1; i >= 0; i--)
                yield return _items[i];
        }
    }

    // ─── Generic Queue ────────────────────────────────────────────────────────

    public class Queue<T>
    {
        private readonly LinkedList<T> _items = new();

        public int  Count   => _items.Count;
        public bool IsEmpty => _items.Count == 0;

        public void Enqueue(T item) => _items.AddLast(item);

        public T Dequeue()
        {
            if (IsEmpty) throw new InvalidOperationException("Queue is empty");
            var front = _items.First!.Value;
            _items.RemoveFirst();
            return front;
        }

        public T Peek()
        {
            if (IsEmpty) throw new InvalidOperationException("Queue is empty");
            return _items.First!.Value;
        }

        public bool TryDequeue(out T? item)
        {
            if (IsEmpty) { item = default; return false; }
            item = Dequeue();
            return true;
        }

        public void Clear() => _items.Clear();
    }

    // ─── Priority Queue (min-heap using SortedDictionary) ────────────────────

    public class PriorityQueue<T> where T : IComparable<T>
    {
        private readonly SortedDictionary<T, Queue<T>> _buckets = new();
        private int _count;

        public int  Count   => _count;
        public bool IsEmpty => _count == 0;

        public void Enqueue(T item)
        {
            if (!_buckets.ContainsKey(item))
                _buckets[item] = new Queue<T>();
            _buckets[item].Enqueue(item);
            _count++;
        }

        public T Dequeue()
        {
            if (IsEmpty) throw new InvalidOperationException("Queue is empty");
            var first = _buckets.First();
            var item  = first.Value.Dequeue();
            if (first.Value.IsEmpty) _buckets.Remove(first.Key);
            _count--;
            return item;
        }
    }

    class Program
    {
        static void Main()
        {
            Console.WriteLine("─── Generic Stack ───────────────────────────────");
            var stack = new Stack<int>();
            stack.Push(1); stack.Push(2); stack.Push(3);
            Console.WriteLine($"  Count: {stack.Count}  Peek: {stack.Peek()}");
            Console.WriteLine($"  Pop: {stack.Pop()}  Pop: {stack.Pop()}");
            Console.WriteLine($"  Count: {stack.Count}");

            Console.WriteLine("\n─── Stack contents (LIFO) ───────────────────────");
            var stack2 = new Stack<string>();
            stack2.Push("first"); stack2.Push("second"); stack2.Push("third");
            foreach (var item in stack2.ToEnumerable())
                Console.WriteLine($"  {item}");

            Console.WriteLine("\n─── Generic Queue ───────────────────────────────");
            var queue = new Queue<string>();
            queue.Enqueue("first"); queue.Enqueue("second"); queue.Enqueue("third");
            Console.WriteLine($"  Count: {queue.Count}  Peek: {queue.Peek()}");
            Console.WriteLine($"  Dequeue: {queue.Dequeue()}  Dequeue: {queue.Dequeue()}");
            Console.WriteLine($"  Count: {queue.Count}");

            Console.WriteLine("\n─── Priority Queue (min first) ───────────────────");
            var pq = new PriorityQueue<int>();
            pq.Enqueue(5); pq.Enqueue(1); pq.Enqueue(3); pq.Enqueue(2);
            while (!pq.IsEmpty)
                Console.Write($"  {pq.Dequeue()}");
            Console.WriteLine();

            Console.WriteLine("\n─── TryPop on empty stack ───────────────────────");
            var empty = new Stack<int>();
            Console.WriteLine($"  TryPop success: {empty.TryPop(out var val)}  value: {val}");
        }
    }
}
