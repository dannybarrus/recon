using System;
using System.Collections.Generic;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Implement a simple IoC container supporting Transient and Singleton lifetimes.
// This was the original challenge that started the Recon repo.

namespace Recon.CodingChallenges
{
    public enum Lifetime { Transient, Singleton }

    public class Container
    {
        private record Registration(Type Implementation, Lifetime Lifetime, object? Instance = null);

        private readonly Dictionary<Type, Registration> _registrations = new();
        private readonly Dictionary<Type, object>       _singletons    = new();

        // ─── Register ─────────────────────────────────────────────────────
        public Container RegisterTransient<TInterface, TImpl>() where TImpl : TInterface
        {
            _registrations[typeof(TInterface)] = new Registration(typeof(TImpl), Lifetime.Transient);
            return this;
        }

        public Container RegisterSingleton<TInterface, TImpl>() where TImpl : TInterface
        {
            _registrations[typeof(TInterface)] = new Registration(typeof(TImpl), Lifetime.Singleton);
            return this;
        }

        public Container RegisterInstance<TInterface>(TInterface instance)
        {
            var type = typeof(TInterface);
            _registrations[type] = new Registration(instance!.GetType(), Lifetime.Singleton);
            _singletons[type]    = instance!;
            return this;
        }

        // ─── Resolve ──────────────────────────────────────────────────────
        public T Resolve<T>() => (T)Resolve(typeof(T));

        private object Resolve(Type type)
        {
            if (!_registrations.TryGetValue(type, out var reg))
                throw new InvalidOperationException($"Type {type.Name} is not registered");

            if (reg.Lifetime == Lifetime.Singleton)
            {
                if (!_singletons.TryGetValue(type, out var existing))
                    _singletons[type] = existing = CreateInstance(reg.Implementation);
                return existing;
            }

            return CreateInstance(reg.Implementation);
        }

        private object CreateInstance(Type type)
        {
            var constructor = type.GetConstructors()[0];
            var parameters  = constructor.GetParameters();
            var args        = new object[parameters.Length];

            for (int i = 0; i < parameters.Length; i++)
                args[i] = Resolve(parameters[i].ParameterType);

            return constructor.Invoke(args);
        }
    }

    // ─── Test classes ─────────────────────────────────────────────────────────

    public interface ILogger     { void Log(string message); }
    public interface IRepository { string Get(int id); }
    public interface IService    { string Execute(int id); }

    public class ConsoleLogger : ILogger
    {
        private readonly Guid _id = Guid.NewGuid();
        public void Log(string message) => Console.WriteLine($"  [Logger {_id:N}[..8]] {message}");
    }

    public class DataRepository : IRepository
    {
        private readonly ILogger _logger;
        public DataRepository(ILogger logger) => _logger = logger;

        public string Get(int id)
        {
            _logger.Log($"Fetching id {id}");
            return $"Data for {id}";
        }
    }

    public class DataService : IService
    {
        private readonly IRepository _repository;
        private readonly ILogger     _logger;

        public DataService(IRepository repository, ILogger logger)
            => (_repository, _logger) = (repository, logger);

        public string Execute(int id)
        {
            _logger.Log($"Executing for id {id}");
            return _repository.Get(id);
        }
    }

    class Program
    {
        static void Main()
        {
            var container = new Container();
            container
                .RegisterSingleton<ILogger,     ConsoleLogger>()
                .RegisterTransient<IRepository, DataRepository>()
                .RegisterTransient<IService,    DataService>();

            Console.WriteLine("─── Resolve and execute ──────────────────────────");
            var service = container.Resolve<IService>();
            Console.WriteLine($"  Result: {service.Execute(42)}");

            Console.WriteLine("\n─── Singleton: same logger instance ─────────────");
            var s1 = container.Resolve<IService>();
            var s2 = container.Resolve<IService>();
            Console.WriteLine("  (Both services share the same logger Guid)");
            s1.Execute(1);
            s2.Execute(2);

            Console.WriteLine("\n─── Transient: new repository each time ──────────");
            var r1 = container.Resolve<IRepository>();
            var r2 = container.Resolve<IRepository>();
            Console.WriteLine($"  Same instance: {ReferenceEquals(r1, r2)}");

            Console.WriteLine("\n─── Unregistered type throws ─────────────────────");
            try { container.Resolve<IDisposable>(); }
            catch (InvalidOperationException ex) { Console.WriteLine($"  Caught: {ex.Message}"); }
        }
    }
}
