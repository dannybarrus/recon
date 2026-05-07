using System;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Convert callback-based APIs to async/await using TaskCompletionSource.
// Real-world use: wrapping legacy event-based code, third-party SDKs.

namespace Recon.CodingChallenges
{
    // ─── Legacy callback-based API (simulates third-party SDK) ───────────────
    public class LegacyDataService
    {
        public void FetchData(string id, Action<string> onSuccess, Action<Exception> onError)
        {
            ThreadPool.QueueUserWorkItem(_ =>
            {
                Thread.Sleep(100);  // simulate async work
                if (id == "bad")
                    onError(new ArgumentException($"Invalid id: {id}"));
                else
                    onSuccess($"Data for {id}");
            });
        }

        // Event-based pattern (EAP)
        public event EventHandler<string>?    DataReceived;
        public event EventHandler<Exception>? ErrorOccurred;

        public void FetchDataEvent(string id)
        {
            ThreadPool.QueueUserWorkItem(_ =>
            {
                Thread.Sleep(100);
                if (id == "bad") ErrorOccurred?.Invoke(this, new ArgumentException($"Bad id: {id}"));
                else             DataReceived?.Invoke(this, $"Event data for {id}");
            });
        }
    }

    // ─── Wrapper: exposes async API over the legacy callback API ─────────────
    public class AsyncDataService
    {
        private readonly LegacyDataService _legacy = new();

        // ─── Callback to Task using TaskCompletionSource ──────────────────
        public Task<string> FetchDataAsync(string id, CancellationToken ct = default)
        {
            var tcs = new TaskCompletionSource<string>(
                TaskCreationOptions.RunContinuationsAsynchronously);

            ct.Register(() => tcs.TrySetCanceled(ct));

            _legacy.FetchData(
                id,
                onSuccess: result  => tcs.TrySetResult(result),
                onError:   ex      => tcs.TrySetException(ex));

            return tcs.Task;
        }

        // ─── Event-based pattern (EAP) to Task ───────────────────────────
        public Task<string> FetchDataEventAsync(string id, CancellationToken ct = default)
        {
            var tcs = new TaskCompletionSource<string>(
                TaskCreationOptions.RunContinuationsAsynchronously);

            ct.Register(() => tcs.TrySetCanceled(ct));

            _legacy.DataReceived  += (_, data) => tcs.TrySetResult(data);
            _legacy.ErrorOccurred += (_, ex)   => tcs.TrySetException(ex);
            _legacy.FetchDataEvent(id);

            return tcs.Task;
        }

        // ─── With timeout: compose CancellationTokenSource ───────────────
        public async Task<string> FetchDataWithTimeoutAsync(string id, TimeSpan timeout)
        {
            using var cts = new CancellationTokenSource(timeout);
            try
            {
                return await FetchDataAsync(id, cts.Token);
            }
            catch (OperationCanceledException)
            {
                throw new TimeoutException($"Fetch timed out after {timeout.TotalMilliseconds}ms");
            }
        }
    }

    class Program
    {
        static async Task Main()
        {
            var service = new AsyncDataService();

            Console.WriteLine("─── Callback to async (success) ──────────────────");
            var result = await service.FetchDataAsync("doc-001");
            Console.WriteLine($"  {result}");

            Console.WriteLine("\n─── Callback to async (error) ────────────────────");
            try { await service.FetchDataAsync("bad"); }
            catch (ArgumentException ex) { Console.WriteLine($"  Caught: {ex.Message}"); }

            Console.WriteLine("\n─── Event-based to async ────────────────────────");
            var eventResult = await service.FetchDataEventAsync("doc-002");
            Console.WriteLine($"  {eventResult}");

            Console.WriteLine("\n─── With cancellation ───────────────────────────");
            using var cts = new CancellationTokenSource(TimeSpan.FromMilliseconds(50));
            try { await service.FetchDataAsync("doc-003", cts.Token); }
            catch (OperationCanceledException) { Console.WriteLine("  Cancelled by caller"); }

            Console.WriteLine("\n─── With timeout ────────────────────────────────");
            try { await service.FetchDataWithTimeoutAsync("doc-004", TimeSpan.FromMilliseconds(50)); }
            catch (TimeoutException ex) { Console.WriteLine($"  {ex.Message}"); }
        }
    }
}
