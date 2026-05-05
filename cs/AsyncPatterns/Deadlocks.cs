namespace Recon.AsyncPatterns
{
    // ─── WHAT CAUSES AN ASYNC DEADLOCK ───────────────────────────────────────
    //
    // Classic deadlock: a synchronisation context is waiting for a Task to
    // complete, while the Task continuation is waiting to resume on that same
    // context. Neither can proceed.
    //
    // Common in: ASP.NET (classic), WPF, WinForms — any environment with a
    // single-threaded SynchronisationContext.
    // Less common in: ASP.NET Core, which has no SynchronisationContext.
    // Still relevant: library code consumed from those environments.

    public class DeadlockExamples
    {
        private readonly IDataService _service;

        public DeadlockExamples(IDataService service) => _service = service;

        // ─── VIOLATION: blocking on async with .Result ────────────────────
        //
        // Thread A (UI/request thread) calls .Result — blocks, holds the context.
        // Task continuation needs that same context to resume.
        // Neither proceeds. Deadlock.

        public string GetDataDeadlock()
        {
            return _service.GetDataAsync().Result;       // NEVER do this
        }

        public string GetDataDeadlockWait()
        {
            var task = _service.GetDataAsync();
            task.Wait();                                 // same problem
            return task.Result;
        }

        // ─── VIOLATION: async void swallows exceptions ────────────────────
        //
        // async void cannot be awaited. Exceptions escape to the
        // SynchronisationContext and crash the process.
        // Only acceptable for event handlers — nowhere else.

        public async void FireAndForgetViolation()
        {
            await _service.GetDataAsync();               // exception is unobservable
        }

        // ─── CORRECT: await all the way down ─────────────────────────────
        //
        // The only safe fix. Async propagates from the innermost call to the
        // outermost entry point. No blocking, no context capture issues.

        public async Task<string> GetDataCorrectAsync(CancellationToken ct = default)
        {
            return await _service.GetDataAsync(ct);
        }

        // ─── CORRECT: ConfigureAwait(false) in library code ──────────────
        //
        // Tells the runtime: do not capture the current SynchronisationContext.
        // The continuation can resume on any thread pool thread.
        // Use in library code — not in application code where context matters.

        public async Task<string> GetDataLibraryAsync(CancellationToken ct = default)
        {
            return await _service.GetDataAsync(ct).ConfigureAwait(false);
        }

        // ─── CORRECT: Task.Run to offload blocking work ───────────────────
        //
        // If you genuinely must call blocking code from an async context,
        // offload it to the thread pool via Task.Run.
        // This is a last resort — prefer async APIs end-to-end.

        public async Task<string> OffloadBlockingWorkAsync(CancellationToken ct = default)
        {
            return await Task.Run(() => SomeLegacyBlockingMethod(), ct);
        }

        private string SomeLegacyBlockingMethod() => "legacy result";

        // ─── CORRECT: async void — the one legitimate use ─────────────────

        public event EventHandler? DataLoaded;

        public async void OnButtonClick(object? sender, EventArgs e)
        {
            // Event handler signature forces async void.
            // Wrap in try/catch — exceptions cannot propagate to the caller.
            try
            {
                var data = await _service.GetDataAsync();
                DataLoaded?.Invoke(this, EventArgs.Empty);
            }
            catch (Exception ex)
            {
                // log ex — do not let it escape
                _ = ex;
            }
        }
    }

    // ─── QUICK REFERENCE ─────────────────────────────────────────────────────
    //
    // Root cause:    .Result / .Wait() blocks a thread that holds a context
    //                the continuation needs to resume on.
    //
    // Fix:           await all the way up — no blocking calls on async code.
    //
    // Library code:  add ConfigureAwait(false) to avoid capturing context.
    //
    // Legacy code:   Task.Run() to offload blocking work to the thread pool.
    //
    // async void:    only for event handlers, always wrap in try/catch.

    public interface IDataService
    {
        Task<string> GetDataAsync(CancellationToken ct = default);
    }
}
