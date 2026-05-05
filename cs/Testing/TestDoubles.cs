namespace Recon.Testing
{
    // ─── THE SYSTEM UNDER TEST ───────────────────────────────────────────────

    public interface IOrderRepository
    {
        Task<Order?> GetByIdAsync(int id, CancellationToken ct = default);
        Task SaveAsync(Order order, CancellationToken ct = default);
    }

    public interface IEmailService
    {
        Task SendConfirmationAsync(string email, string orderRef, CancellationToken ct = default);
    }

    public class OrderService
    {
        private readonly IOrderRepository _repository;
        private readonly IEmailService _emailService;

        public OrderService(IOrderRepository repository, IEmailService emailService)
            => (_repository, _emailService) = (repository, emailService);

        public async Task<bool> ConfirmOrderAsync(int orderId, CancellationToken ct = default)
        {
            var order = await _repository.GetByIdAsync(orderId, ct);
            if (order is null || order.IsConfirmed) return false;

            order.IsConfirmed = true;
            await _repository.SaveAsync(order, ct);
            await _emailService.SendConfirmationAsync(order.CustomerEmail, order.Reference, ct);
            return true;
        }
    }

    // ─── STUB ─────────────────────────────────────────────────────────────────
    //
    // Returns canned data. No assertions. Used to control the input to the SUT.
    // "When asked for order 1, return this order."

    public class StubOrderRepository : IOrderRepository
    {
        private readonly Order? _order;
        public StubOrderRepository(Order? order) => _order = order;

        public Task<Order?> GetByIdAsync(int id, CancellationToken ct = default)
            => Task.FromResult(_order);

        public Task SaveAsync(Order order, CancellationToken ct = default)
            => Task.CompletedTask;   // stub does nothing — no assertion here
    }

    // ─── MOCK ─────────────────────────────────────────────────────────────────
    //
    // Records calls so the test can assert behaviour — was a method called?
    // How many times? With which arguments?
    // "Verify that SendConfirmationAsync was called exactly once."

    public class MockEmailService : IEmailService
    {
        public int SendCount { get; private set; }
        public string? LastEmail { get; private set; }
        public string? LastReference { get; private set; }

        public Task SendConfirmationAsync(string email, string orderRef, CancellationToken ct = default)
        {
            SendCount++;
            LastEmail = email;
            LastReference = orderRef;
            return Task.CompletedTask;
        }
    }

    // ─── FAKE ─────────────────────────────────────────────────────────────────
    //
    // A working implementation with simplified internals — not suitable for
    // production but real enough to support integration-style tests.
    // "An in-memory repository that actually stores and retrieves data."

    public class FakeOrderRepository : IOrderRepository
    {
        private readonly Dictionary<int, Order> _store = new();

        public Task<Order?> GetByIdAsync(int id, CancellationToken ct = default)
            => Task.FromResult(_store.TryGetValue(id, out var order) ? order : null);

        public Task SaveAsync(Order order, CancellationToken ct = default)
        {
            _store[order.Id] = order;
            return Task.CompletedTask;
        }
    }

    // ─── TESTS (xUnit style — no framework dependency needed to read) ─────────

    public class OrderServiceTests
    {
        [Fact_]
        public async Task ConfirmOrder_SendsEmail_WhenOrderExists()
        {
            var order = new Order { Id = 1, CustomerEmail = "dan@example.com", Reference = "ORD-001" };
            var repository = new StubOrderRepository(order);   // stub controls the data
            var emailService = new MockEmailService();          // mock records the call
            var sut = new OrderService(repository, emailService);

            var result = await sut.ConfirmOrderAsync(1);

            _ = result;                         // true — order was confirmed
            _ = emailService.SendCount;         // 1 — email was sent exactly once
            _ = emailService.LastEmail;         // "dan@example.com"
        }

        [Fact_]
        public async Task ConfirmOrder_ReturnsFalse_WhenOrderNotFound()
        {
            var repository = new StubOrderRepository(null);    // stub returns null
            var emailService = new MockEmailService();
            var sut = new OrderService(repository, emailService);

            var result = await sut.ConfirmOrderAsync(99);

            _ = result;                         // false
            _ = emailService.SendCount;         // 0 — no email sent
        }

        [Fact_]
        public async Task ConfirmOrder_PersistsChange_UsingFake()
        {
            var repository = new FakeOrderRepository();        // fake — real behaviour, in memory
            await repository.SaveAsync(new Order { Id = 1, CustomerEmail = "dan@example.com", Reference = "ORD-001" });
            var sut = new OrderService(repository, new MockEmailService());

            await sut.ConfirmOrderAsync(1);

            var saved = await repository.GetByIdAsync(1);
            _ = saved?.IsConfirmed;             // true — state actually persisted
        }
    }

    // ─── QUICK REFERENCE ─────────────────────────────────────────────────────
    //
    // Stub  — controls inputs, returns canned data, no assertions
    // Mock  — records interactions, assertions on behaviour (was X called?)
    // Fake  — working simplified implementation, used for integration-style tests
    //
    // Spy   — like a mock but wraps a real implementation (records calls AND delegates)
    // Dummy — satisfies a parameter signature, never actually used in the test

    // ─── Supporting types ─────────────────────────────────────────────────────

    public class Order
    {
        public int Id { get; set; }
        public string CustomerEmail { get; set; } = string.Empty;
        public string Reference { get; set; } = string.Empty;
        public bool IsConfirmed { get; set; }
    }

    // Placeholder — in a real project this would be xUnit's [Fact] attribute
    [AttributeUsage(AttributeTargets.Method)]
    public class Fact_Attribute : Attribute { }
}
