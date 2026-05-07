using System;
using System.Collections.Concurrent;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement a thread-safe in-memory cache with expiry.
// Variants: absolute expiry, sliding expiry, GetOrAdd pattern.

namespace Recon.CodingChallenges
{
    public class CacheEntry<T>
    {
        public T         Value     { get; }
        public DateTime  ExpiresAt { get; }
        public bool      IsExpired => DateTime.UtcNow >= ExpiresAt;

        public CacheEntry(T value, TimeSpan ttl)
        {
            Value     = value;
            ExpiresAt = DateTime.UtcNow.Add(ttl);
        }
    }

    // ─── Thread-safe cache with absolute TTL ─────────────────────────────────
    public class ExpiringCache<TKey, TValue> where TKey : notnull
    {
        private readonly ConcurrentDictionary<TKey, CacheEntry<TValue>> _store = new();
        private readonly TimeSpan _defaultTtl;

        public ExpiringCache(TimeSpan defaultTtl) => _defaultTtl = defaultTtl;

        public void Set(TKey key, TValue value, TimeSpan? ttl = null)
            => _store[key] = new CacheEntry<TValue>(value, ttl ?? _defaultTtl);

        public bool TryGet(TKey key, out TValue? value)
        {
            if (_store.TryGetValue(key, out var entry) && !entry.IsExpired)
            {
                value = entry.Value;
                return true;
            }

            _store.TryRemove(key, out _);
            value = default;
            return false;
        }

        // ─── GetOrAdd: fetch from source if not cached ────────────────────
        public async Task<TValue> GetOrAddAsync(
            TKey key, Func<TKey, Task<TValue>> factory, TimeSpan? ttl = null)
        {
            if (TryGet(key, out var cached)) return cached!;

            var value = await factory(key);
            Set(key, value, ttl);
            return value;
        }

        public void Remove(TKey key) => _store.TryRemove(key, out _);

        public void Purge()
        {
            foreach (var key in _store.Keys)
                if (_store.TryGetValue(key, out var entry) && entry.IsExpired)
                    _store.TryRemove(key, out _);
        }

        public int Count => _store.Count;
    }

    // ─── Lazy/async initialization: prevents cache stampede ──────────────────
    public class StampedeSafeCache<TKey, TValue> where TKey : notnull
    {
        private readonly ConcurrentDictionary<TKey, Lazy<Task<TValue>>> _store = new();

        public Task<TValue> GetOrAddAsync(TKey key, Func<Task<TValue>> factory)
            => _store.GetOrAdd(key, _ => new Lazy<Task<TValue>>(factory)).Value;

        public void Remove(TKey key) => _store.TryRemove(key, out _);
    }

    class Program
    {
        static async Task Main()
        {
            var cache = new ExpiringCache<string, string>(TimeSpan.FromSeconds(1));

            Console.WriteLine("─── Basic get/set ───────────────────────────────");
            cache.Set("key1", "value1");
            Console.WriteLine($"  Get key1: {(cache.TryGet("key1", out var v) ? v : "miss")}");
            Console.WriteLine($"  Get key2: {(cache.TryGet("key2", out v) ? v : "miss")}");

            Console.WriteLine("\n─── GetOrAdd (simulates DB fetch) ───────────────");
            int fetchCount = 0;
            async Task<string> FetchFromDb(string key)
            {
                fetchCount++;
                await Task.Delay(20);
                return $"db-value-{key}";
            }

            var r1 = await cache.GetOrAddAsync("product:1", FetchFromDb);
            var r2 = await cache.GetOrAddAsync("product:1", FetchFromDb);  // should hit cache
            Console.WriteLine($"  Result 1: {r1}");
            Console.WriteLine($"  Result 2: {r2}");
            Console.WriteLine($"  DB calls: {fetchCount}  (should be 1)");

            Console.WriteLine("\n─── Expiry ───────────────────────────────────────");
            cache.Set("short", "expires-fast", TimeSpan.FromMilliseconds(100));
            Console.WriteLine($"  Before expiry: {(cache.TryGet("short", out v) ? v : "miss")}");
            await Task.Delay(150);
            Console.WriteLine($"  After expiry:  {(cache.TryGet("short", out v) ? v : "miss")}");

            Console.WriteLine("\n─── Purge expired entries ────────────────────────");
            cache.Set("a", "1", TimeSpan.FromMilliseconds(50));
            cache.Set("b", "2", TimeSpan.FromSeconds(60));
            await Task.Delay(100);
            Console.WriteLine($"  Before purge: {cache.Count} entries");
            cache.Purge();
            Console.WriteLine($"  After purge:  {cache.Count} entries");
        }
    }
}
