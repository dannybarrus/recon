namespace Recon.OOP
{
    // ─── WHY IDisposable EXISTS ───────────────────────────────────────────────
    //
    // The garbage collector reclaims managed memory automatically.
    // It does NOT release unmanaged resources: file handles, database connections,
    // network sockets, native memory, COM objects.
    //
    // IDisposable gives types a deterministic way to release those resources
    // as soon as the caller is done — not whenever the GC gets around to it.

    // ─── BASIC IMPLEMENTATION ─────────────────────────────────────────────────

    public class FileProcessor : IDisposable
    {
        private readonly FileStream _stream;
        private bool _disposed;

        public FileProcessor(string path)
            => _stream = new FileStream(path, FileMode.Open, FileAccess.Read);

        public string ReadLine()
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            return string.Empty; // simplified
        }

        public void Dispose()
        {
            if (_disposed) return;
            _stream.Dispose();
            _disposed = true;
            GC.SuppressFinalize(this);  // tell GC the finalizer is not needed
        }
    }

    // ─── FULL PATTERN: managed + unmanaged resources ──────────────────────────
    //
    // Use when the class directly wraps unmanaged resources (IntPtr, SafeHandle).
    // Most application code only needs the simple version above.

    public class NativeResourceWrapper : IDisposable
    {
        private IntPtr _handle;         // unmanaged
        private IDisposable? _managed;  // managed
        private bool _disposed;

        public NativeResourceWrapper()
        {
            _handle = /* AllocateNativeResource() */ IntPtr.Zero;
            _managed = new FileProcessor("dummy.txt");
        }

        protected virtual void Dispose(bool disposing)
        {
            if (_disposed) return;

            if (disposing)
                _managed?.Dispose();    // release managed resources only when called from Dispose()

            // always release unmanaged resources
            if (_handle != IntPtr.Zero)
            {
                // FreeNativeResource(_handle);
                _handle = IntPtr.Zero;
            }

            _disposed = true;
        }

        public void Dispose()
        {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        ~NativeResourceWrapper()
        {
            Dispose(disposing: false);  // finalizer safety net — managed refs may be gone
        }
    }

    // ─── ASYNC DISPOSAL ───────────────────────────────────────────────────────
    //
    // IAsyncDisposable for resources that require async cleanup —
    // database connections, HTTP clients, stream flushes.

    public class AsyncDatabaseConnection : IAsyncDisposable
    {
        private bool _disposed;

        public async ValueTask DisposeAsync()
        {
            if (_disposed) return;
            await FlushAndCloseAsync();
            _disposed = true;
            GC.SuppressFinalize(this);
        }

        private Task FlushAndCloseAsync() => Task.CompletedTask;
    }

    // ─── CALLER PATTERNS ─────────────────────────────────────────────────────

    public class CallerExamples
    {
        public void UsingStatement()
        {
            // Dispose() called automatically at end of block — even if exception thrown
            using var processor = new FileProcessor("data.txt");
            var line = processor.ReadLine();
        }

        public void UsingDeclaration()
        {
            // C# 8 — Dispose() called at end of enclosing scope
            using var processor = new FileProcessor("data.txt");
            // processor is available for the rest of this method
        }

        public async Task AsyncUsing()
        {
            // DisposeAsync() called automatically
            await using var connection = new AsyncDatabaseConnection();
        }

        public void ManualDispose()
        {
            // Equivalent to using — always in a finally block
            var processor = new FileProcessor("data.txt");
            try { var line = processor.ReadLine(); }
            finally { processor.Dispose(); }
        }
    }

    // ─── QUICK REFERENCE ─────────────────────────────────────────────────────
    //
    // IDisposable        — synchronous cleanup of unmanaged resources
    // IAsyncDisposable   — async cleanup (DbConnection, HttpClient, streams)
    // using statement    — guaranteed Dispose() call, even on exception
    // GC.SuppressFinalize — skip the finalizer when Dispose() already ran
    // _disposed flag     — guard against double-disposal
    // Finalizer (~T)     — safety net only; never rely on it for correctness
    //
    // Rule: if your class owns an IDisposable, your class should be IDisposable
}
