using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Recon.Interfaces
{
    public class User
    {
        public int Id { get; set; }
        public string Name { get; set; } = string.Empty;
        public string Email { get; set; } = string.Empty;
        public bool IsActive { get; set; }
    }

    public class UserRepository : IRepository<User>
    {
        private readonly List<User> _store = new();

        public Task<User?> GetByIdAsync(int id, CancellationToken cancellationToken = default)
            => Task.FromResult(_store.FirstOrDefault(u => u.Id == id));

        public Task<IEnumerable<User>> GetAllAsync(CancellationToken cancellationToken = default)
            => Task.FromResult<IEnumerable<User>>(_store);

        public Task AddAsync(User entity, CancellationToken cancellationToken = default)
        {
            _store.Add(entity);
            return Task.CompletedTask;
        }

        public Task UpdateAsync(User entity, CancellationToken cancellationToken = default)
        {
            var index = _store.FindIndex(u => u.Id == entity.Id);
            if (index >= 0) _store[index] = entity;
            return Task.CompletedTask;
        }

        public Task DeleteAsync(int id, CancellationToken cancellationToken = default)
        {
            _store.RemoveAll(u => u.Id == id);
            return Task.CompletedTask;
        }
    }
}
