using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement retry logic with exponential backoff and jitter.
// Real-world use: calling ML inference APIs, external document processors,
// any transient-failure-prone HTTP dependency.

namespace Recon.CodingChallenges
{
    public class RetryPolicy
    {
        private readonly int      _maxAttempts;
        private readonly TimeSpan _baseDelay;
        private readonly double   _backoffMultiplier;
        private readonly TimeSpan _maxDelay;
        private readonly bool     _useJitter;

        public RetryPolicy(
            int      maxAttempts        = 3,
            int      baseDelayMs        = 500,
            double   backoffMultiplier  = 2.0,
            int      maxDelayMs         = 10_000,
            bool     useJitter          = true)
        {
            _maxAttempts       = maxAttempts;
            _baseDelay         = TimeSpan.FromMilliseconds(baseDelayMs);
            _backoffMultiplier = backoffMultiplier;
            _maxDelay          = TimeSpan.FromMilliseconds(maxDelayMs);
            _useJitter         = useJitter;
        }

        // ─── Core retry with exponential backoff ──────────────────────────
        public async Task<T> ExecuteAsync<T>(
            Func<CancellationToken, Task<T>> operation,
            Func<Exception, bool>?           shouldRetry = null,
            CancellationToken                ct          = default)
        {
            shouldRetry ??= IsTransient;

            var delays   = new List<double>();
            var attempts = 0;

            while (true)
            {
                try
                {
                    attempts++;
                    return await operation(ct);
                }
                catch (Exception ex) when (attempts < _maxAttempts && shouldRetry(ex))
                {
                    var delay = CalculateDelay(attempts);
                    delays.Add(delay.TotalMilliseconds);
                    Console.WriteLine($"  Attempt {attempts} failed: {ex.Message}. Retrying in {delay.TotalMilliseconds:N0}ms...");
                    await Task.Delay(delay, ct);
                }
            }
        }

        // ─── Void overload ────────────────────────────────────────────────
        public async Task ExecuteAsync(
            Func<CancellationToken, Task> operation,
            Func<Exception, bool>?        shouldRetry = null,
            CancellationToken             ct          = default)
        {
            await ExecuteAsync<object?>(
                async token => { await operation(token); return null; },
                shouldRetry, ct);
        }

        // ─── Delay calculation: exponential backoff with optional jitter ──
        private TimeSpan CalculateDelay(int attempt)
        {
            var exponential = _baseDelay.TotalMilliseconds * Math.Pow(_backoffMultiplier, attempt - 1);
            var capped      = Math.Min(exponential, _maxDelay.TotalMilliseconds);

            if (_useJitter)
            {
                // Full jitter: random value between 0 and the calculated delay.
                // Prevents thundering herd when many clients retry simultaneously.
                capped = Random.Shared.NextDouble() * capped;
            }

            return TimeSpan.FromMilliseconds(capped);
        }

        // ─── Default: retry on transient exceptions only ──────────────────
        private static bool IsTransient(Exception ex) => ex is
            TimeoutException          or
            InvalidOperationException or   // simulates HTTP 429/503
            TaskCanceledException;
    }

    // ─── Simulated unreliable service ─────────────────────────────────────────
    public class UnreliableDocumentProcessor
    {
        private int _callCount;
        private readonly int _succeedOnAttempt;

        public UnreliableDocumentProcessor(int succeedOnAttempt = 3)
            => _succeedOnAttempt = succeedOnAttempt;

        public async Task<string> ProcessAsync(string documentId, CancellationToken ct = default)
        {
            _callCount++;
            await Task.Delay(50, ct);

            if (_callCount < _succeedOnAttempt)
                throw new InvalidOperationException($"Service unavailable (attempt {_callCount})");

            return $"Processed: {documentId}";
        }

        public void Reset() => _callCount = 0;
    }

    class Program
    {
        static async Task Main()
        {
            var processor = new UnreliableDocumentProcessor(succeedOnAttempt: 3);
            var policy    = new RetryPolicy(maxAttempts: 5, baseDelayMs: 100, useJitter: false);

            Console.WriteLine("─── Retry with exponential backoff ──────────────");
            var result = await policy.ExecuteAsync(
                ct => processor.ProcessAsync("DOC-001", ct));
            Console.WriteLine($"  Final result: {result}");

            Console.WriteLine("\n─── Non-retryable exception (stops immediately) ──");
            processor.Reset();
            var strictPolicy = new RetryPolicy(maxAttempts: 5, baseDelayMs: 100, useJitter: false);

            try
            {
                await strictPolicy.ExecuteAsync(
                    ct => processor.ProcessAsync("DOC-002", ct),
                    shouldRetry: ex => ex is TimeoutException);   // only retry on timeout
            }
            catch (InvalidOperationException ex)
            {
                Console.WriteLine($"  Not retried: {ex.Message}");
            }

            Console.WriteLine("\n─── Exceeds max attempts ─────────────────────────");
            var neverSucceeds = new UnreliableDocumentProcessor(succeedOnAttempt: 99);
            var limitedPolicy = new RetryPolicy(maxAttempts: 3, baseDelayMs: 50, useJitter: false);

            try
            {
                await limitedPolicy.ExecuteAsync(ct => neverSucceeds.ProcessAsync("DOC-003", ct));
            }
            catch (InvalidOperationException ex)
            {
                Console.WriteLine($"  Gave up after max attempts: {ex.Message}");
            }
        }
    }
}
