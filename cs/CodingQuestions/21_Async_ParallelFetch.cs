using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Fetch multiple items in parallel and collect results.
// Variants: all-or-nothing, partial success, bounded parallelism.

namespace Recon.CodingChallenges
{
    public class DocumentResult
    {
        public int    Id      { get; set; }
        public string Content { get; set; } = string.Empty;
        public bool   Success { get; set; }
        public string? Error  { get; set; }
    }

    public class ParallelFetch
    {
        private readonly IDocumentService _service;

        public ParallelFetch(IDocumentService service) => _service = service;

        // ─── All-or-nothing: throws if any fetch fails ────────────────────
        public async Task<IEnumerable<DocumentResult>> FetchAllAsync(
            IEnumerable<int> ids, CancellationToken ct = default)
        {
            var tasks   = ids.Select(id => _service.FetchAsync(id, ct));
            var results = await Task.WhenAll(tasks);    // throws AggregateException if any task throws
            return results;
        }

        // ─── Partial success: failed fetches return an error result ───────
        public async Task<IEnumerable<DocumentResult>> FetchWithPartialSuccessAsync(
            IEnumerable<int> ids, CancellationToken ct = default)
        {
            var tasks = ids.Select(async id =>
            {
                try
                {
                    return await _service.FetchAsync(id, ct);
                }
                catch (Exception ex)
                {
                    return new DocumentResult { Id = id, Success = false, Error = ex.Message };
                }
            });

            return await Task.WhenAll(tasks);
        }

        // ─── Bounded parallelism: limit concurrent requests ───────────────
        // Prevents overwhelming downstream services or hitting rate limits.
        public async Task<IEnumerable<DocumentResult>> FetchBoundedAsync(
            IEnumerable<int> ids, int maxConcurrency = 3, CancellationToken ct = default)
        {
            using var semaphore = new SemaphoreSlim(maxConcurrency);
            var results         = new List<DocumentResult>();

            var tasks = ids.Select(async id =>
            {
                await semaphore.WaitAsync(ct);
                try   { return await _service.FetchAsync(id, ct); }
                finally { semaphore.Release(); }
            });

            return await Task.WhenAll(tasks);
        }

        // ─── First success wins ───────────────────────────────────────────
        // Useful for redundant endpoints or fallback providers.
        public async Task<DocumentResult> FetchFirstSuccessAsync(
            IEnumerable<int> ids, CancellationToken ct = default)
        {
            var tasks = ids.Select(id => _service.FetchAsync(id, ct)).ToList();
            return await Task.WhenAny(tasks).Unwrap();
        }
    }

    // ─── Simulated service ────────────────────────────────────────────────────
    public interface IDocumentService
    {
        Task<DocumentResult> FetchAsync(int id, CancellationToken ct = default);
    }

    public class SimulatedDocumentService : IDocumentService
    {
        private static readonly Random _rng = new();

        public async Task<DocumentResult> FetchAsync(int id, CancellationToken ct = default)
        {
            // Simulate variable latency
            await Task.Delay(_rng.Next(50, 300), ct);

            // Simulate occasional failure
            if (id == 3) throw new InvalidOperationException($"Document {id} not found");

            return new DocumentResult { Id = id, Content = $"Content for document {id}", Success = true };
        }
    }

    class Program
    {
        static async Task Main()
        {
            var service = new SimulatedDocumentService();
            var fetcher = new ParallelFetch(service);
            var ids     = new[] { 1, 2, 4, 5 };          // exclude 3 to avoid failure in all-or-nothing
            var allIds  = new[] { 1, 2, 3, 4, 5 };

            Console.WriteLine("─── All-or-nothing fetch ────────────────────────");
            var sw = System.Diagnostics.Stopwatch.StartNew();
            var results = await fetcher.FetchAllAsync(ids);
            sw.Stop();
            foreach (var r in results)
                Console.WriteLine($"  [{r.Id}] {r.Content}");
            Console.WriteLine($"  Completed in {sw.ElapsedMilliseconds}ms (parallel, not sequential)");

            Console.WriteLine("\n─── Partial success fetch (includes id 3) ───────");
            var partial = await fetcher.FetchWithPartialSuccessAsync(allIds);
            foreach (var r in partial)
            {
                if (r.Success) Console.WriteLine($"  [{r.Id}] OK: {r.Content}");
                else           Console.WriteLine($"  [{r.Id}] FAILED: {r.Error}");
            }

            Console.WriteLine("\n─── Bounded parallelism (max 2 concurrent) ──────");
            sw.Restart();
            var bounded = await fetcher.FetchBoundedAsync(ids, maxConcurrency: 2);
            sw.Stop();
            Console.WriteLine($"  Fetched {bounded.Count()} documents in {sw.ElapsedMilliseconds}ms");
        }
    }
}
