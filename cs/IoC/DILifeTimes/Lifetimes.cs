using Microsoft.Extensions.DependencyInjection;

namespace Recon.IoC
{
    // ─── THE THREE LIFETIMES ──────────────────────────────────────────────────
    //
    // Transient  — new instance every time it is requested
    // Scoped     — one instance per scope (one per HTTP request in ASP.NET Core)
    // Singleton  — one instance for the lifetime of the application

    public interface ILifetimeDemo
    {
        Guid InstanceId { get; }
    }

    public class TransientService  : ILifetimeDemo { public Guid InstanceId { get; } = Guid.NewGuid(); }
    public class ScopedService     : ILifetimeDemo { public Guid InstanceId { get; } = Guid.NewGuid(); }
    public class SingletonService  : ILifetimeDemo { public Guid InstanceId { get; } = Guid.NewGuid(); }

    public static class LifetimeRegistration
    {
        public static IServiceCollection AddLifetimeExamples(this IServiceCollection services)
        {
            services.AddTransient<TransientService>();
            services.AddScoped<ScopedService>();
            services.AddSingleton<SingletonService>();
            return services;
        }
    }

    // ─── WHAT CHANGES BETWEEN REQUESTS ───────────────────────────────────────
    //
    // Request 1:  Transient A → new Guid    Scoped A → new Guid    Singleton → same Guid
    // Request 1:  Transient B → new Guid    Scoped B → same Guid   Singleton → same Guid
    //
    // Request 2:  Transient C → new Guid    Scoped C → new Guid    Singleton → same Guid
    //             (Scoped resets per request; Singleton never changes)

    // ─── CAPTIVE DEPENDENCY — THE CRITICAL MISTAKE ───────────────────────────
    //
    // A longer-lived service capturing a shorter-lived dependency.
    // The short-lived service is effectively promoted to the longer lifetime,
    // which is almost always wrong and often causes stale data or concurrency bugs.

    public class CaptiveDependencyViolation
    {
        // WRONG: Singleton holds a reference to a Scoped service.
        // The Scoped service is created once and never refreshed —
        // it outlives its intended scope.
        private readonly ScopedService _scoped;

        public CaptiveDependencyViolation(ScopedService scoped)
            => _scoped = scoped;
    }

    // ─── CORRECT PATTERNS PER LIFETIME ───────────────────────────────────────

    // Transient: stateless, lightweight helpers — validators, formatters, calculators.
    // A new instance is cheap and avoids shared state entirely.
    public class PriceFormatter
    {
        public string Format(decimal amount) => amount.ToString("C2");
    }

    // Scoped: anything that wraps a unit of work — DbContext, repositories,
    // services that track per-request state. One per request keeps data consistent
    // within a request without leaking between requests.
    public class OrderRepository
    {
        // In production this would wrap a DbContext (also Scoped).
        // Both share the same scope → same transaction, same change tracker.
        public Task<Order?> GetAsync(int id, CancellationToken ct = default)
            => Task.FromResult<Order?>(null);
    }

    // Singleton: shared, thread-safe, expensive-to-construct resources.
    // HttpClient (via IHttpClientFactory), in-memory caches, configuration
    // wrappers. Must be thread-safe — multiple requests hit it concurrently.
    public class ExchangeRateCache
    {
        private readonly Dictionary<string, decimal> _rates = new();
        private readonly SemaphoreSlim _lock = new(1, 1);

        public async Task<decimal> GetRateAsync(string currency, CancellationToken ct = default)
        {
            await _lock.WaitAsync(ct);
            try { return _rates.TryGetValue(currency, out var rate) ? rate : 1m; }
            finally { _lock.Release(); }
        }
    }

    // ─── QUICK DECISION GUIDE ────────────────────────────────────────────────
    //
    // Does it wrap a DbContext or unit of work?  → Scoped
    // Is it stateless and cheap to create?       → Transient
    // Is it thread-safe and expensive to build?  → Singleton
    // Does it hold user-specific state?          → never Singleton

    public class Order { public int Id { get; set; } }
}
