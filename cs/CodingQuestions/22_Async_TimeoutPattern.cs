using System;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement async operations with timeouts using CancellationTokenSource.
// Variants: simple timeout, linked tokens, timeout with fallback.

namespace Recon.CodingChallenges
{
    public class TimeoutPatterns
    {
        private readonly ISlowService _service;

        public TimeoutPatterns(ISlowService service) => _service = service;

        // ─── Simple timeout ───────────────────────────────────────────────
        public async Task<string> GetWithTimeoutAsync(int id, TimeSpan timeout)
        {
            using var cts = new CancellationTokenSource(timeout);

            try
            {
                return await _service.GetAsync(id, cts.Token);
            }
            catch (OperationCanceledException)
            {
                throw new TimeoutException($"Request for id {id} timed out after {timeout.TotalSeconds}s");
            }
        }

        // ─── Linked tokens: caller can cancel OR timeout can fire ─────────
        // Respects both the caller's cancellation and the timeout deadline.
        public async Task<string> GetWithLinkedCancellationAsync(
            int id, TimeSpan timeout, CancellationToken callerToken = default)
        {
            using var timeoutCts = new CancellationTokenSource(timeout);
            using var linkedCts  = CancellationTokenSource.CreateLinkedTokenSource(
                                       callerToken, timeoutCts.Token);
            try
            {
                return await _service.GetAsync(id, linkedCts.Token);
            }
            catch (OperationCanceledException) when (timeoutCts.IsCancellationRequested)
            {
                throw new TimeoutException($"Request timed out after {timeout.TotalSeconds}s");
            }
            catch (OperationCanceledException)
            {
                throw;  // caller cancelled intentionally, propagate
            }
        }

        // ─── Timeout with fallback value ──────────────────────────────────
        // Returns a default if the operation times out rather than throwing.
        public async Task<string> GetWithFallbackAsync(
            int id, TimeSpan timeout, string fallback = "default")
        {
            using var cts = new CancellationTokenSource(timeout);

            try
            {
                return await _service.GetAsync(id, cts.Token);
            }
            catch (OperationCanceledException)
            {
                return fallback;
            }
        }

        // ─── Timeout via Task.WhenAny ─────────────────────────────────────
        // Alternative pattern: race the operation against a delay.
        // Less preferred — leaks the original task, which continues running.
        // Shown here for completeness; prefer CancellationTokenSource above.
        public async Task<string> GetWithWhenAnyAsync(int id, TimeSpan timeout)
        {
            var operationTask = _service.GetAsync(id);
            var timeoutTask   = Task.Delay(timeout);

            var winner = await Task.WhenAny(operationTask, timeoutTask);

            if (winner == timeoutTask)
                throw new TimeoutException($"Request timed out after {timeout.TotalSeconds}s");

            return await operationTask;
        }
    }

    // ─── Simulated slow service ───────────────────────────────────────────────
    public interface ISlowService
    {
        Task<string> GetAsync(int id, CancellationToken ct = default);
    }

    public class SimulatedSlowService : ISlowService
    {
        private readonly int _delayMs;

        public SimulatedSlowService(int delayMs) => _delayMs = delayMs;

        public async Task<string> GetAsync(int id, CancellationToken ct = default)
        {
            await Task.Delay(_delayMs, ct);     // ct.ThrowIfCancellationRequested() called by Task.Delay
            return $"Result for {id}";
        }
    }

    class Program
    {
        static async Task Main()
        {
            Console.WriteLine("─── Simple timeout (fast service, succeeds) ─────");
            var fast    = new SimulatedSlowService(delayMs: 100);
            var fastOps = new TimeoutPatterns(fast);

            var result = await fastOps.GetWithTimeoutAsync(1, TimeSpan.FromSeconds(2));
            Console.WriteLine($"  Result: {result}");

            Console.WriteLine("\n─── Simple timeout (slow service, times out) ────");
            var slow    = new SimulatedSlowService(delayMs: 2000);
            var slowOps = new TimeoutPatterns(slow);

            try
            {
                await slowOps.GetWithTimeoutAsync(1, TimeSpan.FromMilliseconds(500));
            }
            catch (TimeoutException ex)
            {
                Console.WriteLine($"  Caught: {ex.Message}");
            }

            Console.WriteLine("\n─── Timeout with fallback ────────────────────────");
            var fallback = await slowOps.GetWithFallbackAsync(1, TimeSpan.FromMilliseconds(500), "cached-value");
            Console.WriteLine($"  Fallback result: {fallback}");

            Console.WriteLine("\n─── Linked cancellation (caller cancels early) ───");
            using var callerCts = new CancellationTokenSource(TimeSpan.FromMilliseconds(200));
            try
            {
                await slowOps.GetWithLinkedCancellationAsync(1, TimeSpan.FromSeconds(5), callerCts.Token);
            }
            catch (OperationCanceledException)
            {
                Console.WriteLine("  Caller cancelled the request");
            }
            catch (TimeoutException ex)
            {
                Console.WriteLine($"  Timed out: {ex.Message}");
            }
        }
    }
}
