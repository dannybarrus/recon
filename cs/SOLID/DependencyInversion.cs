using Microsoft.Extensions.DependencyInjection;

namespace Recon.SOLID
{
    // VIOLATION: high-level module hardcodes a dependency on a low-level concretion
    public class ReportServiceViolation
    {
        private readonly SqlReportRepository _repository = new();  // hardcoded

        public async Task<string> GenerateAsync(int id)
        {
            var data = await _repository.GetAsync(id);
            return $"Report: {data}";
        }
    }

    public class SqlReportRepository
    {
        public Task<string> GetAsync(int id) => Task.FromResult($"sql-data-{id}");
    }

    // CORRECT: both high and low-level modules depend on the abstraction
    public interface IReportRepository
    {
        Task<string> GetAsync(int id, CancellationToken ct = default);
    }

    public class SqlReportRepositoryCorrect : IReportRepository
    {
        public Task<string> GetAsync(int id, CancellationToken ct = default)
            => Task.FromResult($"sql-data-{id}");
    }

    public class CachedReportRepository : IReportRepository
    {
        private readonly IReportRepository _inner;

        public CachedReportRepository(IReportRepository inner)
            => _inner = inner;

        public async Task<string> GetAsync(int id, CancellationToken ct = default)
        {
            // check cache first, delegate to inner on miss
            return await _inner.GetAsync(id, ct);
        }
    }

    public class ReportService
    {
        private readonly IReportRepository _repository;

        public ReportService(IReportRepository repository)
            => _repository = repository;

        public async Task<string> GenerateAsync(int id, CancellationToken ct = default)
        {
            var data = await _repository.GetAsync(id, ct);
            return $"Report: {data}";
        }
    }

    public static class DipServiceRegistration
    {
        public static IServiceCollection AddReporting(this IServiceCollection services)
        {
            services.AddScoped<IReportRepository, SqlReportRepositoryCorrect>();
            services.AddScoped<ReportService>();
            return services;
        }
    }
}
