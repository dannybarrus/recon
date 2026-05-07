using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Refactor a fat class applying SRP, then extend it using the Decorator pattern.
// Two challenges in one: 13_OOP_SOLIDRefactor and 15_OOP_Decorator.

namespace Recon.CodingChallenges
{
    // ═══════════════════════════════════════════════════════════════════════════
    // BEFORE: SRP violation — one class owns too many responsibilities
    // ═══════════════════════════════════════════════════════════════════════════

    public class OrderServiceViolation
    {
        public void ProcessOrder(Order order)
        {
            // validate
            if (order.Total <= 0) throw new ArgumentException("Invalid total");

            // persist
            Console.WriteLine($"  Saving order {order.Id} to database");

            // notify
            Console.WriteLine($"  Sending email to {order.CustomerEmail}");

            // audit log
            Console.WriteLine($"  Writing audit log entry for order {order.Id}");
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // AFTER: SRP applied — each class owns one responsibility
    // ═══════════════════════════════════════════════════════════════════════════

    public class Order
    {
        public int     Id            { get; set; }
        public decimal Total         { get; set; }
        public string  CustomerEmail { get; set; } = string.Empty;
    }

    public class OrderValidator
    {
        public void Validate(Order order)
        {
            if (order.Total <= 0) throw new ArgumentException("Total must be positive");
        }
    }

    public class OrderRepository
    {
        public void Save(Order order)
            => Console.WriteLine($"  [Repo]     Saved order {order.Id}");
    }

    public class OrderNotifier
    {
        public void Notify(Order order)
            => Console.WriteLine($"  [Notifier] Sent confirmation to {order.CustomerEmail}");
    }

    public class AuditLogger
    {
        public void Log(Order order)
            => Console.WriteLine($"  [Audit]    Logged order {order.Id} at {DateTime.UtcNow:HH:mm:ss}");
    }

    // Thin coordinator — no business logic, orchestration only
    public class OrderProcessor
    {
        private readonly OrderValidator  _validator;
        private readonly OrderRepository _repository;
        private readonly OrderNotifier   _notifier;
        private readonly AuditLogger     _auditor;

        public OrderProcessor(
            OrderValidator validator, OrderRepository repository,
            OrderNotifier notifier,  AuditLogger auditor)
            => (_validator, _repository, _notifier, _auditor)
             = (validator,  repository,  notifier,  auditor);

        public void Process(Order order)
        {
            _validator.Validate(order);
            _repository.Save(order);
            _notifier.Notify(order);
            _auditor.Log(order);
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // DECORATOR PATTERN: add logging and caching without touching existing code
    // ═══════════════════════════════════════════════════════════════════════════

    public interface IProductRepository
    {
        Task<string?> GetByIdAsync(int id, CancellationToken ct = default);
    }

    // Base implementation
    public class SqlProductRepository : IProductRepository
    {
        public async Task<string?> GetByIdAsync(int id, CancellationToken ct = default)
        {
            await Task.Delay(50, ct);    // simulate DB latency
            return id > 0 ? $"Product {id}" : null;
        }
    }

    // Decorator 1: logging — wraps any IProductRepository
    public class LoggingProductRepository : IProductRepository
    {
        private readonly IProductRepository _inner;

        public LoggingProductRepository(IProductRepository inner) => _inner = inner;

        public async Task<string?> GetByIdAsync(int id, CancellationToken ct = default)
        {
            Console.WriteLine($"  [Log]   Fetching product {id}");
            var result = await _inner.GetByIdAsync(id, ct);
            Console.WriteLine($"  [Log]   Result: {result ?? "null"}");
            return result;
        }
    }

    // Decorator 2: caching — wraps any IProductRepository
    public class CachingProductRepository : IProductRepository
    {
        private readonly IProductRepository  _inner;
        private readonly Dictionary<int, string?> _cache = new();

        public CachingProductRepository(IProductRepository inner) => _inner = inner;

        public async Task<string?> GetByIdAsync(int id, CancellationToken ct = default)
        {
            if (_cache.TryGetValue(id, out var cached))
            {
                Console.WriteLine($"  [Cache] HIT for product {id}");
                return cached;
            }

            Console.WriteLine($"  [Cache] MISS for product {id} — fetching");
            var result = await _inner.GetByIdAsync(id, ct);
            _cache[id] = result;
            return result;
        }
    }

    class Program
    {
        static async Task Main()
        {
            Console.WriteLine("─── SOLID refactor (SRP) ────────────────────────");
            var processor = new OrderProcessor(
                new OrderValidator(), new OrderRepository(),
                new OrderNotifier(),  new AuditLogger());

            processor.Process(new Order { Id = 1, Total = 99.99m, CustomerEmail = "dan@example.com" });

            Console.WriteLine("\n─── Decorator: base only ────────────────────────");
            IProductRepository repo = new SqlProductRepository();
            Console.WriteLine(await repo.GetByIdAsync(1));

            Console.WriteLine("\n─── Decorator: logging + base ───────────────────");
            repo = new LoggingProductRepository(new SqlProductRepository());
            await repo.GetByIdAsync(1);

            Console.WriteLine("\n─── Decorator: caching + logging + base ──────────");
            repo = new CachingProductRepository(
                       new LoggingProductRepository(
                           new SqlProductRepository()));

            await repo.GetByIdAsync(1);   // miss, fetches
            await repo.GetByIdAsync(1);   // hit, no fetch
            await repo.GetByIdAsync(2);   // miss, fetches
        }
    }
}
