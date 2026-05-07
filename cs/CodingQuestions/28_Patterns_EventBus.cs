using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement a simple in-process event bus (pub/sub).
// Type-safe handlers, async support, unsubscribe tokens.

namespace Recon.CodingChallenges
{
    // ─── Event marker interface ───────────────────────────────────────────────
    public interface IEvent { }

    // ─── Handler interface ────────────────────────────────────────────────────
    public interface IEventHandler<in TEvent> where TEvent : IEvent
    {
        Task HandleAsync(TEvent @event, CancellationToken ct = default);
    }

    // ─── Subscription token for unsubscribing ─────────────────────────────────
    public class Subscription : IDisposable
    {
        private readonly Action _unsubscribe;
        private bool _disposed;

        public Subscription(Action unsubscribe) => _unsubscribe = unsubscribe;

        public void Dispose()
        {
            if (_disposed) return;
            _unsubscribe();
            _disposed = true;
        }
    }

    // ─── Event bus ───────────────────────────────────────────────────────────
    public class EventBus
    {
        private readonly ConcurrentDictionary<Type, List<Func<IEvent, CancellationToken, Task>>> _handlers = new();
        private readonly object _lock = new();

        // ─── Subscribe with typed handler ─────────────────────────────────
        public Subscription Subscribe<TEvent>(IEventHandler<TEvent> handler)
            where TEvent : IEvent
        {
            Func<IEvent, CancellationToken, Task> wrapper =
                (e, ct) => handler.HandleAsync((TEvent)e, ct);

            return Subscribe<TEvent>(wrapper);
        }

        // ─── Subscribe with inline delegate ───────────────────────────────
        public Subscription Subscribe<TEvent>(Func<TEvent, CancellationToken, Task> handler)
            where TEvent : IEvent
        {
            Func<IEvent, CancellationToken, Task> wrapper =
                (e, ct) => handler((TEvent)e, ct);

            return Subscribe<TEvent>(wrapper);
        }

        private Subscription Subscribe<TEvent>(Func<IEvent, CancellationToken, Task> handler)
            where TEvent : IEvent
        {
            var type = typeof(TEvent);
            lock (_lock)
            {
                if (!_handlers.ContainsKey(type))
                    _handlers[type] = new List<Func<IEvent, CancellationToken, Task>>();
                _handlers[type].Add(handler);
            }
            return new Subscription(() =>
            {
                lock (_lock) { _handlers[type].Remove(handler); }
            });
        }

        // ─── Publish: notifies all subscribers ────────────────────────────
        public async Task PublishAsync<TEvent>(TEvent @event, CancellationToken ct = default)
            where TEvent : IEvent
        {
            var type = typeof(TEvent);
            List<Func<IEvent, CancellationToken, Task>> handlers;

            lock (_lock)
            {
                if (!_handlers.TryGetValue(type, out var list)) return;
                handlers = list.ToList();  // snapshot to avoid holding lock during async calls
            }

            await Task.WhenAll(handlers.Select(h => h(@event, ct)));
        }
    }

    // ─── Sample events ────────────────────────────────────────────────────────

    public record OrderPlacedEvent(int OrderId, string Customer, decimal Total) : IEvent;
    public record PaymentProcessedEvent(int OrderId, bool Success) : IEvent;

    // ─── Sample handlers ──────────────────────────────────────────────────────

    public class EmailNotificationHandler : IEventHandler<OrderPlacedEvent>
    {
        public Task HandleAsync(OrderPlacedEvent e, CancellationToken ct = default)
        {
            Console.WriteLine($"  [Email]     Sending confirmation to {e.Customer} for order {e.OrderId}");
            return Task.CompletedTask;
        }
    }

    public class InventoryHandler : IEventHandler<OrderPlacedEvent>
    {
        public Task HandleAsync(OrderPlacedEvent e, CancellationToken ct = default)
        {
            Console.WriteLine($"  [Inventory] Reserving items for order {e.OrderId}");
            return Task.CompletedTask;
        }
    }

    public class AuditHandler : IEventHandler<PaymentProcessedEvent>
    {
        public Task HandleAsync(PaymentProcessedEvent e, CancellationToken ct = default)
        {
            Console.WriteLine($"  [Audit]     Payment {(e.Success ? "succeeded" : "failed")} for order {e.OrderId}");
            return Task.CompletedTask;
        }
    }

    class Program
    {
        static async Task Main()
        {
            var bus = new EventBus();

            Console.WriteLine("─── Subscribe and publish ────────────────────────");
            bus.Subscribe<OrderPlacedEvent>(new EmailNotificationHandler());
            bus.Subscribe<OrderPlacedEvent>(new InventoryHandler());
            bus.Subscribe<PaymentProcessedEvent>(new AuditHandler());

            await bus.PublishAsync(new OrderPlacedEvent(1, "Alice", 99.99m));
            await bus.PublishAsync(new PaymentProcessedEvent(1, true));

            Console.WriteLine("\n─── Inline delegate subscription ────────────────");
            bus.Subscribe<OrderPlacedEvent>(async (e, ct) =>
            {
                await Task.Delay(10, ct);
                Console.WriteLine($"  [Analytics] Tracked order {e.OrderId} worth ${e.Total}");
            });

            await bus.PublishAsync(new OrderPlacedEvent(2, "Bob", 249.99m));

            Console.WriteLine("\n─── Unsubscribe via token ────────────────────────");
            using (var sub = bus.Subscribe<OrderPlacedEvent>(async (e, _) =>
            {
                await Task.CompletedTask;
                Console.WriteLine($"  [Temp]      Temporary handler fired for order {e.OrderId}");
            }))
            {
                await bus.PublishAsync(new OrderPlacedEvent(3, "Carol", 49.99m));
                Console.WriteLine("  (Unsubscribing temp handler)");
            }  // sub.Dispose() called here

            Console.WriteLine("\n─── After unsubscribe (temp handler silent) ──────");
            await bus.PublishAsync(new OrderPlacedEvent(4, "Dave", 19.99m));
        }
    }
}
