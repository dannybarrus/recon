using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement the Repository pattern for a simple domain model.
// Show: interface, concrete implementation, service consumer.

namespace Recon.CodingChallenges
{
    // ─── Domain model ─────────────────────────────────────────────────────────
    public class Product
    {
        public int     Id       { get; set; }
        public string  Name     { get; set; } = string.Empty;
        public decimal Price    { get; set; }
        public int     Stock    { get; set; }
        public bool    IsActive { get; set; } = true;
    }

    // ─── Repository interface ─────────────────────────────────────────────────
    public interface IProductRepository
    {
        Task<Product?>              GetByIdAsync(int id, CancellationToken ct = default);
        Task<IEnumerable<Product>>  GetAllAsync(CancellationToken ct = default);
        Task<IEnumerable<Product>>  FindAsync(Func<Product, bool> predicate, CancellationToken ct = default);
        Task                        AddAsync(Product product, CancellationToken ct = default);
        Task                        UpdateAsync(Product product, CancellationToken ct = default);
        Task                        DeleteAsync(int id, CancellationToken ct = default);
    }

    // ─── In-memory implementation ─────────────────────────────────────────────
    public class InMemoryProductRepository : IProductRepository
    {
        private readonly List<Product> _store = new();
        private int _nextId = 1;

        public Task<Product?> GetByIdAsync(int id, CancellationToken ct = default)
            => Task.FromResult(_store.FirstOrDefault(p => p.Id == id));

        public Task<IEnumerable<Product>> GetAllAsync(CancellationToken ct = default)
            => Task.FromResult<IEnumerable<Product>>(_store.Where(p => p.IsActive));

        public Task<IEnumerable<Product>> FindAsync(Func<Product, bool> predicate, CancellationToken ct = default)
            => Task.FromResult(_store.Where(predicate));

        public Task AddAsync(Product product, CancellationToken ct = default)
        {
            product.Id = _nextId++;
            _store.Add(product);
            return Task.CompletedTask;
        }

        public Task UpdateAsync(Product product, CancellationToken ct = default)
        {
            var index = _store.FindIndex(p => p.Id == product.Id);
            if (index >= 0) _store[index] = product;
            return Task.CompletedTask;
        }

        public Task DeleteAsync(int id, CancellationToken ct = default)
        {
            // Soft delete — preserves history, safer than hard delete
            var product = _store.FirstOrDefault(p => p.Id == id);
            if (product is not null) product.IsActive = false;
            return Task.CompletedTask;
        }
    }

    // ─── Service layer — business logic lives here, not in the repository ─────
    public class ProductService
    {
        private readonly IProductRepository _repository;

        public ProductService(IProductRepository repository)
            => _repository = repository;

        public async Task<IEnumerable<Product>> GetInStockAsync(CancellationToken ct = default)
        {
            var products = await _repository.GetAllAsync(ct);
            return products.Where(p => p.Stock > 0);
        }

        public async Task<bool> AdjustStockAsync(int productId, int delta, CancellationToken ct = default)
        {
            var product = await _repository.GetByIdAsync(productId, ct);
            if (product is null) return false;

            var newStock = product.Stock + delta;
            if (newStock < 0) return false;     // business rule: stock cannot go negative

            product.Stock = newStock;
            await _repository.UpdateAsync(product, ct);
            return true;
        }

        public async Task<IEnumerable<Product>> SearchAsync(string term, CancellationToken ct = default)
        {
            if (string.IsNullOrWhiteSpace(term)) return Enumerable.Empty<Product>();

            return await _repository.FindAsync(
                p => p.Name.Contains(term, StringComparison.OrdinalIgnoreCase), ct);
        }
    }

    class Program
    {
        static async Task Main()
        {
            var repository = new InMemoryProductRepository();
            var service    = new ProductService(repository);

            // Seed
            await repository.AddAsync(new Product { Name = "Widget A", Price = 9.99m,  Stock = 100 });
            await repository.AddAsync(new Product { Name = "Widget B", Price = 19.99m, Stock = 0   });
            await repository.AddAsync(new Product { Name = "Gadget X", Price = 49.99m, Stock = 25  });
            await repository.AddAsync(new Product { Name = "Gadget Y", Price = 29.99m, Stock = 10  });

            Console.WriteLine("─── All active products ─────────────────────────");
            foreach (var p in await repository.GetAllAsync())
                Console.WriteLine($"  [{p.Id}] {p.Name,-12} ${p.Price:N2}  Stock: {p.Stock}");

            Console.WriteLine("\n─── In stock only ───────────────────────────────");
            foreach (var p in await service.GetInStockAsync())
                Console.WriteLine($"  {p.Name,-12} Stock: {p.Stock}");

            Console.WriteLine("\n─── Search: 'gadget' ────────────────────────────");
            foreach (var p in await service.SearchAsync("gadget"))
                Console.WriteLine($"  {p.Name}");

            Console.WriteLine("\n─── Adjust stock for product 1 (-30) ────────────");
            var success = await service.AdjustStockAsync(1, -30);
            Console.WriteLine($"  Success: {success}");
            var updated = await repository.GetByIdAsync(1);
            Console.WriteLine($"  New stock: {updated?.Stock}");

            Console.WriteLine("\n─── Soft delete product 2 ───────────────────────");
            await repository.DeleteAsync(2);
            var all = await repository.GetAllAsync();
            Console.WriteLine($"  Active products: {all.Count()}  (Widget B excluded)");
        }
    }
}
