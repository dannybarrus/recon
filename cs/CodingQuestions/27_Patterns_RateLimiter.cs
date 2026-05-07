using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement a rate limiter. Allow N calls per time window.
// Variants: fixed window, sliding window, token bucket.

namespace Recon.CodingChallenges
{
    // ─── Fixed window: simple, but allows burst at window boundary ────────────
    public class FixedWindowRateLimiter
    {
        private readonly int      _limit;
        private readonly TimeSpan _window;
        private int               _count;
        private DateTime          _windowStart = DateTime.UtcNow;
        private readonly object   _lock = new();

        public FixedWindowRateLimiter(int limit, TimeSpan window)
            => (_limit, _window) = (limit, window);

        public bool TryAcquire()
        {
            lock (_lock)
            {
                var now = DateTime.UtcNow;
                if (now - _windowStart >= _window)
                {
                    _windowStart = now;
                    _count       = 0;
                }

                if (_count >= _limit) return false;
                _count++;
                return true;
            }
        }
    }

    // ─── Sliding window: smoother, prevents boundary bursts ──────────────────
    public class SlidingWindowRateLimiter
    {
        private readonly int       _limit;
        private readonly TimeSpan  _window;
        private readonly Queue<DateTime> _timestamps = new();
        private readonly object    _lock = new();

        public SlidingWindowRateLimiter(int limit, TimeSpan window)
            => (_limit, _window) = (limit, window);

        public bool TryAcquire()
        {
            lock (_lock)
            {
                var now = DateTime.UtcNow;
                var cutoff = now - _window;

                while (_timestamps.Count > 0 && _timestamps.Peek() < cutoff)
                    _timestamps.Dequeue();

                if (_timestamps.Count >= _limit) return false;
                _timestamps.Enqueue(now);
                return true;
            }
        }

        public int CurrentCount
        {
            get
            {
                lock (_lock)
                {
                    var cutoff = DateTime.UtcNow - _window;
                    while (_timestamps.Count > 0 && _timestamps.Peek() < cutoff)
                        _timestamps.Dequeue();
                    return _timestamps.Count;
                }
            }
        }
    }

    // ─── Token bucket: allows controlled bursting ─────────────────────────────
    public class TokenBucketRateLimiter
    {
        private readonly int      _capacity;
        private readonly double   _refillRatePerMs;
        private double            _tokens;
        private DateTime          _lastRefill = DateTime.UtcNow;
        private readonly object   _lock = new();

        public TokenBucketRateLimiter(int capacity, int refillPerSecond)
        {
            _capacity        = capacity;
            _tokens          = capacity;
            _refillRatePerMs = refillPerSecond / 1000.0;
        }

        public bool TryAcquire(int tokensRequired = 1)
        {
            lock (_lock)
            {
                Refill();
                if (_tokens < tokensRequired) return false;
                _tokens -= tokensRequired;
                return true;
            }
        }

        private void Refill()
        {
            var now     = DateTime.UtcNow;
            var elapsed = (now - _lastRefill).TotalMilliseconds;
            _tokens     = Math.Min(_capacity, _tokens + elapsed * _refillRatePerMs);
            _lastRefill = now;
        }
    }

    // ─── Per-client rate limiter using a dictionary of limiters ──────────────
    public class PerClientRateLimiter
    {
        private readonly ConcurrentDictionary<string, SlidingWindowRateLimiter> _clients = new();
        private readonly int      _limit;
        private readonly TimeSpan _window;

        public PerClientRateLimiter(int limit, TimeSpan window)
            => (_limit, _window) = (limit, window);

        public bool TryAcquire(string clientId)
            => _clients.GetOrAdd(clientId, _ => new SlidingWindowRateLimiter(_limit, _window))
                       .TryAcquire();
    }

    class Program
    {
        static async Task Main()
        {
            Console.WriteLine("─── Sliding window (5 per second) ───────────────");
            var limiter = new SlidingWindowRateLimiter(5, TimeSpan.FromSeconds(1));

            for (int i = 1; i <= 8; i++)
            {
                var allowed = limiter.TryAcquire();
                Console.WriteLine($"  Request {i}: {(allowed ? "ALLOWED" : "DENIED ")}  (window count: {limiter.CurrentCount})");
            }

            Console.WriteLine("\n─── After 1 second, window resets ───────────────");
            await Task.Delay(1100);
            for (int i = 1; i <= 3; i++)
                Console.WriteLine($"  Request {i}: {(limiter.TryAcquire() ? "ALLOWED" : "DENIED")}");

            Console.WriteLine("\n─── Token bucket (capacity 3, refill 2/sec) ──────");
            var bucket = new TokenBucketRateLimiter(capacity: 3, refillPerSecond: 2);
            for (int i = 1; i <= 5; i++)
            {
                Console.WriteLine($"  Request {i}: {(bucket.TryAcquire() ? "ALLOWED" : "DENIED")}");
                await Task.Delay(200);   // 200ms between calls, refill rate 2/sec = 0.4 tokens per 200ms
            }

            Console.WriteLine("\n─── Per-client rate limiting ─────────────────────");
            var perClient = new PerClientRateLimiter(3, TimeSpan.FromSeconds(5));
            var clients   = new[] { "alice", "alice", "alice", "alice", "bob", "bob" };
            foreach (var client in clients)
                Console.WriteLine($"  {client,-8}: {(perClient.TryAcquire(client) ? "ALLOWED" : "DENIED")}");
        }
    }
}
