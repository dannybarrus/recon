using System.Runtime.CompilerServices;

namespace Recon.AsyncPatterns
{
    public class AsyncPatterns
    {
        private readonly IUserDataSource _source;

        public AsyncPatterns(IUserDataSource source)
            => _source = source;

        // Basic async/await — never use .Result or .Wait()
        public async Task<User?> GetUserAsync(int id, CancellationToken ct = default)
        {
            var user = await _source.FetchAsync(id, ct);
            return user is { IsActive: true } ? user : null;
        }

        // ValueTask — prefer when the result is frequently cached/synchronous
        public async ValueTask<int> GetUserCountAsync(CancellationToken ct = default)
        {
            if (_source.IsCached) return _source.CachedCount;
            return await _source.CountAsync(ct);
        }

        // ConfigureAwait(false) — avoid capturing context in library code
        public async Task<string> LibraryMethodAsync(CancellationToken ct = default)
        {
            var data = await _source.FetchRawAsync(ct).ConfigureAwait(false);
            return data.ToString();
        }

        // IAsyncEnumerable — stream results without buffering the full set
        public async IAsyncEnumerable<User> StreamActiveUsersAsync(
            [EnumeratorCancellation] CancellationToken ct = default)
        {
            await foreach (var user in _source.StreamAsync(ct))
            {
                if (user.IsActive) yield return user;
            }
        }

        // Parallel async — fan out, then collect
        public async Task<IEnumerable<User>> GetUsersParallelAsync(
            IEnumerable<int> ids, CancellationToken ct = default)
        {
            var tasks = ids.Select(id => _source.FetchAsync(id, ct));
            var results = await Task.WhenAll(tasks);
            return results.Where(u => u is not null)!;
        }

        // Timeout via CancellationTokenSource
        public async Task<User?> GetWithTimeoutAsync(int id)
        {
            using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(5));
            return await _source.FetchAsync(id, cts.Token);
        }
    }

    // ─── Supporting types ────────────────────────────────────────────────────

    public class User
    {
        public int Id { get; set; }
        public bool IsActive { get; set; }
    }

    public interface IUserDataSource
    {
        bool IsCached { get; }
        int CachedCount { get; }
        Task<User?> FetchAsync(int id, CancellationToken ct = default);
        Task<int> CountAsync(CancellationToken ct = default);
        Task<object> FetchRawAsync(CancellationToken ct = default);
        IAsyncEnumerable<User> StreamAsync(CancellationToken ct = default);
    }
}
