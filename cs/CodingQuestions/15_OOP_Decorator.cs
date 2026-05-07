using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement the Decorator pattern to add cross-cutting concerns
// (logging, caching, timing, retry) without modifying existing classes.

namespace Recon.CodingChallenges
{
    public interface IWeatherService
    {
        Task<string> GetForecastAsync(string city, CancellationToken ct = default);
    }

    // ─── Base implementation ──────────────────────────────────────────────────
    public class WeatherService : IWeatherService
    {
        public async Task<string> GetForecastAsync(string city, CancellationToken ct = default)
        {
            await Task.Delay(100, ct);   // simulate external API call
            return $"Sunny, 72F in {city}";
        }
    }

    // ─── Decorator 1: logging ─────────────────────────────────────────────────
    public class LoggingWeatherService : IWeatherService
    {
        private readonly IWeatherService _inner;
        public LoggingWeatherService(IWeatherService inner) => _inner = inner;

        public async Task<string> GetForecastAsync(string city, CancellationToken ct = default)
        {
            Console.WriteLine($"  [LOG]   Requesting forecast for {city}");
            var result = await _inner.GetForecastAsync(city, ct);
            Console.WriteLine($"  [LOG]   Received: {result}");
            return result;
        }
    }

    // ─── Decorator 2: timing ──────────────────────────────────────────────────
    public class TimingWeatherService : IWeatherService
    {
        private readonly IWeatherService _inner;
        public TimingWeatherService(IWeatherService inner) => _inner = inner;

        public async Task<string> GetForecastAsync(string city, CancellationToken ct = default)
        {
            var sw = Stopwatch.StartNew();
            var result = await _inner.GetForecastAsync(city, ct);
            sw.Stop();
            Console.WriteLine($"  [TIMER] {city} took {sw.ElapsedMilliseconds}ms");
            return result;
        }
    }

    // ─── Decorator 3: caching ─────────────────────────────────────────────────
    public class CachingWeatherService : IWeatherService
    {
        private readonly IWeatherService       _inner;
        private readonly Dictionary<string, string> _cache = new();

        public CachingWeatherService(IWeatherService inner) => _inner = inner;

        public async Task<string> GetForecastAsync(string city, CancellationToken ct = default)
        {
            if (_cache.TryGetValue(city, out var cached))
            {
                Console.WriteLine($"  [CACHE] HIT for {city}");
                return cached;
            }

            Console.WriteLine($"  [CACHE] MISS for {city}");
            var result = await _inner.GetForecastAsync(city, ct);
            _cache[city] = result;
            return result;
        }
    }

    // ─── Decorator 4: retry ───────────────────────────────────────────────────
    public class RetryWeatherService : IWeatherService
    {
        private readonly IWeatherService _inner;
        private readonly int             _maxAttempts;

        public RetryWeatherService(IWeatherService inner, int maxAttempts = 3)
            => (_inner, _maxAttempts) = (inner, maxAttempts);

        public async Task<string> GetForecastAsync(string city, CancellationToken ct = default)
        {
            for (int attempt = 1; attempt <= _maxAttempts; attempt++)
            {
                try
                {
                    return await _inner.GetForecastAsync(city, ct);
                }
                catch (Exception ex) when (attempt < _maxAttempts)
                {
                    Console.WriteLine($"  [RETRY] Attempt {attempt} failed: {ex.Message}");
                    await Task.Delay(100 * attempt, ct);
                }
            }
            throw new InvalidOperationException($"Failed after {_maxAttempts} attempts");
        }
    }

    class Program
    {
        static async Task Main()
        {
            Console.WriteLine("─── Base service only ───────────────────────────");
            IWeatherService svc = new WeatherService();
            Console.WriteLine($"  {await svc.GetForecastAsync("Seattle")}");

            Console.WriteLine("\n─── Logging + base ───────────────────────────────");
            svc = new LoggingWeatherService(new WeatherService());
            await svc.GetForecastAsync("Portland");

            Console.WriteLine("\n─── Timing + logging + base ──────────────────────");
            svc = new TimingWeatherService(
                      new LoggingWeatherService(
                          new WeatherService()));
            await svc.GetForecastAsync("Denver");

            Console.WriteLine("\n─── Caching + timing + logging + base ────────────");
            svc = new CachingWeatherService(
                      new TimingWeatherService(
                          new LoggingWeatherService(
                              new WeatherService())));

            await svc.GetForecastAsync("Austin");   // miss
            await svc.GetForecastAsync("Austin");   // hit — no inner calls
            await svc.GetForecastAsync("Dallas");   // miss
        }
    }
}
