using Microsoft.Extensions.DependencyInjection;

namespace Recon.Interfaces
{
    public static class ServiceRegistration
    {
        public static IServiceCollection AddRepositories(this IServiceCollection services)
        {
            services.AddScoped<IRepository<User>, UserRepository>();
            return services;
        }
    }

    // Consumer — depends on abstraction, never on concretion
    public class UserService
    {
        private readonly IRepository<User> _repository;

        public UserService(IRepository<User> repository)
            => _repository = repository;

        public async Task<User?> GetActiveUserAsync(int id, CancellationToken ct = default)
        {
            var user = await _repository.GetByIdAsync(id, ct);
            return user is { IsActive: true } ? user : null;
        }
    }
}
